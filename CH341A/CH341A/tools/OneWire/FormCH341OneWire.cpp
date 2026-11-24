//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341OneWire.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341OneWire *frmCH341OneWire;

REGISTER_TOOL(TfrmCH341OneWire, frmCH341OneWire, 1u << ToolGroupGenericInterfaces)
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

AnsiString BytesToString(const uint8_t *data, unsigned int count)
{
	AnsiString s;
	for (unsigned int i = 0; i < count; i++)
	{
		if (i != 0)
			s += " ";
		s += IntToHex(static_cast<int>(data[i]), 2);
	}
	return s;
}

/** Hex bytes separated by spaces / commas, e.g. "CC 44" or "CC,44" or "CC44" */
bool ParseHexBytes(const AnsiString &text, std::vector<uint8_t> &data)
{
	data.clear();
	int nibbles = 0;
	int value = 0;
	for (int i = 1; i <= text.Length(); i++)
	{
		const char c = text[i];
		int digit;
		if (c >= '0' && c <= '9')
			digit = c - '0';
		else if (c >= 'a' && c <= 'f')
			digit = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			digit = c - 'A' + 10;
		else if (c == ' ' || c == ',' || c == ';' || c == '\t')
		{
			if (nibbles != 0)
				return false;	// byte split by a separator
			continue;
		}
		else
			return false;
		value = (value << 4) | digit;
		if (++nibbles == 2)
		{
			data.push_back(static_cast<uint8_t>(value));
			nibbles = 0;
			value = 0;
		}
	}
	return nibbles == 0;
}

}

//---------------------------------------------------------------------------

__fastcall TfrmCH341OneWire::TfrmCH341OneWire(TComponent* Owner)
	: TForm(Owner)
{
	memInfo->Lines->Text =
		"Generic 1-Wire master (reset / presence, ROM commands, raw bytes). "
		"Pulse widths are generated using CH341 UIO stream, thus D0...D5 pins can be used.\r\n"
		"\r\n"
		"Connection: 1-Wire DQ (data)  ->  D0 = SPI CS0 (default), or D3 = SPI CLK or D5 = SPI MOSI\r\n"
		"pull-up ~4.7k from DQ to the same voltage as the 1-Wire device supply (3.3 V tested)\r\n"
		"Parasite powered devices (VDD to GND) might need the stronger pull-up.";
}
//---------------------------------------------------------------------------

void TfrmCH341OneWire::Log(const AnsiString &text)
{
	memLog->Lines->Add(text);
}

bool TfrmCH341OneWire::Prepare(void)
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
	int low = edReadLow->Text.ToIntDef(2);
	int wait = edReadWait->Text.ToIntDef(6);
	if (low < 1)
		low = 1;
	if (wait < 1)
		wait = 1;
	ow.SetReadTiming(static_cast<unsigned int>(low), static_cast<unsigned int>(wait));
	return true;
}

void TfrmCH341OneWire::Report(int status)
{
	AnsiString text = CH341OneWire::ErrorText(status);
	lblStatus->Caption = text;
	Log(text);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341OneWire::btnCheckClick(TObject *Sender)
{
	BtnController btnCtrl(btnCheck);
	if (!Prepare())
		return;
	bool high = false;
	int status = ow.IdleLevel(high);
	if (status != CH341OneWire::OK)
	{
		Report(status);
		return;
	}
	AnsiString text = high ? "DQ is high (pull-up present, bus idle)" :
		"DQ is LOW: short circuit, no pull-up, wrong pin, or a device holding the line";
	lblStatus->Caption = text;
	Log(text);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341OneWire::btnResetClick(TObject *Sender)
{
	BtnController btnCtrl(btnReset);
	if (!Prepare())
		return;
	bool present = false;
	int status = ow.Reset(present);
	if (status != CH341OneWire::OK)
	{
		Report(status);
		return;
	}
	AnsiString text = present ? "Presence pulse detected" : "No presence pulse: no device answering";
	lblStatus->Caption = text;
	Log(text);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341OneWire::btnReadRomClick(TObject *Sender)
{
	BtnController btnCtrl(btnReadRom);
	if (!Prepare())
		return;
	OneWireRom rom;
	int status = ow.ReadRom(rom);
	if (status != CH341OneWire::OK)
	{
		Report(status);
		return;
	}
	AnsiString text = "ROM " + RomToString(rom) + " - " + CH341OneWire::FamilyName(rom.id[0]);
	lblStatus->Caption = "ROM read OK (CRC OK)";
	Log(text);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341OneWire::btnSearchClick(TObject *Sender)
{
	BtnController btnCtrl(btnSearch);
	if (!Prepare())
		return;
	std::vector<OneWireRom> roms;
	int status = ow.Search(roms);
	if (status != CH341OneWire::OK)
	{
		Report(status);
		return;
	}
	Log(AnsiString().sprintf("Search ROM: %u device(s)", static_cast<unsigned int>(roms.size())));
	for (size_t i = 0; i < roms.size(); i++)
		Log("  " + RomToString(roms[i]) + " - " + CH341OneWire::FamilyName(roms[i].id[0]));
	lblStatus->Caption = AnsiString().sprintf("%u device(s) found", static_cast<unsigned int>(roms.size()));
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341OneWire::btnWriteReadClick(TObject *Sender)
{
	BtnController btnCtrl(btnWriteRead);
	if (!Prepare())
		return;
	std::vector<uint8_t> data;
	if (!ParseHexBytes(edWrite->Text, data))
	{
		lblStatus->Caption = "Invalid hex data to write (use e.g. \"CC 44\")";
		return;
	}
	int readCount = edReadCount->Text.ToIntDef(-1);
	if (readCount < 0 || readCount > 256)
	{
		lblStatus->Caption = "Number of bytes to read must be 0..256";
		return;
	}
	if (chbResetFirst->Checked)
	{
		bool present = false;
		int status = ow.Reset(present);
		if (status != CH341OneWire::OK)
		{
			Report(status);
			return;
		}
		if (!present)
		{
			Report(CH341OneWire::ERR_NO_PRESENCE);
			return;
		}
	}
	if (!data.empty())
	{
		int status = ow.WriteBytes(&data[0], static_cast<unsigned int>(data.size()));
		if (status != CH341OneWire::OK)
		{
			Report(status);
			return;
		}
		Log("Wrote: " + BytesToString(&data[0], static_cast<unsigned int>(data.size())));
	}
	if (readCount > 0)
	{
		std::vector<uint8_t> rx(static_cast<size_t>(readCount));
		int status = ow.ReadBytes(&rx[0], static_cast<unsigned int>(rx.size()));
		if (status != CH341OneWire::OK)
		{
			Report(status);
			return;
		}
		Log("Read:  " + BytesToString(&rx[0], static_cast<unsigned int>(rx.size())));
	}
	lblStatus->Caption = "Done";
}
//---------------------------------------------------------------------------
