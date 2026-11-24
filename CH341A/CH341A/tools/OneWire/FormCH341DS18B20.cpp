//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341DS18B20.h"
#include "Ds18b20.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341DS18B20 *frmCH341DS18B20;

REGISTER_TOOL(TfrmCH341DS18B20, frmCH341DS18B20, 1u << ToolGroupSensors)
//---------------------------------------------------------------------------

namespace
{

// pins easily accessible on the CH341A-based programmer, in the order of the cbPin items
const unsigned int PIN_FOR_ITEM[] = { 0, 3, 5 };

AnsiString RomToString(const OneWireRom &rom)
{
	AnsiString s;
	for (int i = 0; i < 8; i++)
	{
		if (i != 0)
			s += ":";
		s += IntToHex(static_cast<int>(rom.id[i]), 2);
	}
	return s;
}

bool IsTemperatureFamily(uint8_t family)
{
	return family == 0x28 || family == 0x22 || family == 0x10;
}

}

//---------------------------------------------------------------------------

__fastcall TfrmCH341DS18B20::TfrmCH341DS18B20(TComponent* Owner)
	: TForm(Owner)
{
	memInfo->Lines->Text =
		"DS18B20 / DS1822 / DS18S20 (DS1820) 1-Wire temperature sensors. "
		"Generic 1-Wire master (reset / presence, ROM commands, raw bytes). "
		"Pulse widths are generated using CH341 UIO stream, thus D0...D5 pins can be used.\r\n"
		"\r\n"
		"Connection: 1-Wire DQ (data)  ->  D0 = SPI CS0 (default) or D3 = SPI CLK or D5 = SPI MOSI\r\n"
		"pull-up ~4.7k from DQ to the same voltage as the 1-Wire device supply (3.3 V tested).\r\n"
		"Parasite powered devices (VDD to GND) might need the stronger pull-up.";
		"Several sensors can share the same DQ line: use Scan bus and select the sensor.";
}
//---------------------------------------------------------------------------

bool TfrmCH341DS18B20::Prepare(void)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return false;
	}
	int item = cbPin->ItemIndex;
	if (item < 0 || item >= static_cast<int>(sizeof(PIN_FOR_ITEM) / sizeof(PIN_FOR_ITEM[0])))
		item = 0;
	ow.SetPin(PIN_FOR_ITEM[item]);
	return true;
}

void TfrmCH341DS18B20::ShowError(int status)
{
	lblStatus->Caption = CH341OneWire::ErrorText(status);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS18B20::btnScanClick(TObject *Sender)
{
	BtnController btnCtrl(btnScan);
	if (!Prepare())
		return;
	std::vector<OneWireRom> all;
	int status = ow.Search(all);
	if (status != CH341OneWire::OK)
	{
		ShowError(status);
		return;
	}
	devices.clear();
	cbDevice->Items->Clear();
	cbDevice->Items->Add("Single device on the bus (Skip ROM)");
	unsigned int others = 0;
	for (size_t i = 0; i < all.size(); i++)
	{
		if (IsTemperatureFamily(all[i].id[0]))
		{
			devices.push_back(all[i]);
			cbDevice->Items->Add(RomToString(all[i]) + "  " + CH341OneWire::FamilyName(all[i].id[0]));
		}
		else
			others++;
	}
	cbDevice->ItemIndex = devices.empty() ? 0 : 1;
	AnsiString text = AnsiString().sprintf("%u temperature sensor(s) found", static_cast<unsigned int>(devices.size()));
	if (others != 0)
		text.cat_printf(", %u other 1-Wire device(s) ignored", others);
	lblStatus->Caption = text;
}
//---------------------------------------------------------------------------

bool TfrmCH341DS18B20::GetTarget(const OneWireRom *&rom, OneWireRom &single, uint8_t &family)
{
	const int item = cbDevice->ItemIndex;
	if (item >= 1 && item <= static_cast<int>(devices.size()))
	{
		rom = &devices[static_cast<size_t>(item - 1)];
		family = rom->id[0];
		return true;
	}
	// single device: learn the family with Read ROM
	int status = ow.ReadRom(single);
	if (status != CH341OneWire::OK)
	{
		ShowError(status);
		return false;
	}
	if (!IsTemperatureFamily(single.id[0]))
	{
		lblStatus->Caption = AnsiString("The device is not a temperature sensor: ") +
			CH341OneWire::FamilyName(single.id[0]);
		return false;
	}
	rom = NULL;
	family = single.id[0];
	return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS18B20::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341DS18B20::Read(void)
{
	BtnController btnCtrl(btnRead);
	if (!Prepare())
		return;

	const OneWireRom *rom = NULL;
	OneWireRom single;
	uint8_t family = 0;
	if (!GetTarget(rom, single, family))
	{
		lblTemperature->Caption = "";
		lblDetails->Caption = "";
		return;
	}

	Ds18b20 ds(ow);
	Ds18b20Result result;
	// this blocks for the conversion time (up to 750 ms at 12 bits)
	int status = ds.ReadTemperature(rom, family, chbParasite->Checked, result);
	if (status != CH341OneWire::OK)
	{
		lblTemperature->Caption = "";
		lblDetails->Caption = "";
		ShowError(status);
		if (status == CH341OneWire::ERR_TIMEOUT && !chbParasite->Checked)
			lblStatus->Caption = lblStatus->Caption + " (parasite powered sensor? tick the parasite option)";
		return;
	}
	lblTemperature->Caption = AnsiString().sprintf("%.2f C", result.temperature);
	AnsiString details;
	details.cat_printf("Resolution: %d bit", result.resolutionBits);
	if (rom)
		details += ",  ROM " + RomToString(*rom);
	else
		details += ",  ROM " + RomToString(single);
	details += "\r\nScratchpad:";
	for (int i = 0; i < 9; i++)
		details += " " + IntToHex(static_cast<int>(result.scratchpad[i]), 2);
	if (result.powerOnValue)
		details += "\r\nNote: 85.0 C is the power-on value - the conversion may not have happened (check supply / parasite option).";
	lblDetails->Caption = details;
	lblStatus->Caption = "Temperature read OK (CRC OK)";
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS18B20::btnSetResolutionClick(TObject *Sender)
{
	BtnController btnCtrl(btnSetResolution);
	if (!Prepare())
		return;
	const OneWireRom *rom = NULL;
	OneWireRom single;
	uint8_t family = 0;
	if (!GetTarget(rom, single, family))
		return;
	if (family == 0x10)
	{
		lblStatus->Caption = "DS18S20 / DS1820 has fixed 9-bit resolution";
		return;
	}
	const unsigned int bits = static_cast<unsigned int>(cbResolution->ItemIndex + 9);
	Ds18b20 ds(ow);
	int status = ds.SetResolution(rom, bits, chbStore->Checked);
	if (status != CH341OneWire::OK)
	{
		ShowError(status);
		return;
	}
	lblStatus->Caption = AnsiString().sprintf("Resolution set to %u bit%s", bits,
		chbStore->Checked ? " and stored in the sensor EEPROM" : " (until power cycle)");
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS18B20::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
	{
		Read();
		tmrAutoRead->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS18B20::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS18B20::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------
