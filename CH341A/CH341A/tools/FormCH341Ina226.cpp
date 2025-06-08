//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341Ina226.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include "common/ArraySize.h"
#include "ValueDescription.h"
#include <assert.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341Ina226 *frmCH341Ina226;
//---------------------------------------------------------------------------

namespace
{

enum { I2C_START_ADDRESS = 0x40 };
enum { I2C_ADDRESS_COUNT = 16 };

/* Register bytes are sent most-significant byte first, followed by the least significant byte. */

enum {
	REG_CONFIG = 0x00,
	REG_SHUNT_VOLTAGE,
	REG_BUS_VOLTAGE,
	REG_POWER,
	REG_CURRENT,
	REG_CALIBRATION,
	REG_MASK_ENABLE,
	REG_ALERT_LIMIT,

	REG_MANUFACTURER_ID = 0xFE,
	REG_DIE_ID = 0xFF
};

const uint16_t CONFIG_RST = (1<<15);
enum { CONFIG_AVG_OFFSET = 9 };
const uint16_t CONFIG_AVG_1 = 0x0000;
const uint16_t CONFIG_AVG_4 = 0x0001;
const uint16_t CONFIG_AVG_16 = 0x0002;
const uint16_t CONFIG_AVG_64 = 0x0003;
const uint16_t CONFIG_AVG_128 = 0x0004;
const uint16_t CONFIG_AVG_256 = 0x0005;
const uint16_t CONFIG_AVG_512 = 0x0006;
const uint16_t CONFIG_AVG_1024 = 0x0007;
ValueDescriptionU16 avgSel[] = {
	{ CONFIG_AVG_1, "1 sample" },
	{ CONFIG_AVG_4, "4 samples" },
	{ CONFIG_AVG_16, "16 samples" },
	{ CONFIG_AVG_64, "64 samples" },
	{ CONFIG_AVG_128, "128 samples" },
	{ CONFIG_AVG_256, "256 samples" },
	{ CONFIG_AVG_512, "512 samples" },
	{ CONFIG_AVG_1024, "1024 samples" },
};

enum { CONFIG_VBUS_CONV_TIME_OFFSET = 6 };
enum { CONFIG_SHUNT_CONV_TIME_OFFSET = 3 };
// identical/common values for VBUS and SHUNT conversion
const uint16_t CONFIG_CONV_TIME_140US = 0x0000;
const uint16_t CONFIG_CONV_TIME_204US = 0x0001;
const uint16_t CONFIG_CONV_TIME_332US = 0x0002;
const uint16_t CONFIG_CONV_TIME_588US = 0x0003;
const uint16_t CONFIG_CONV_TIME_1100US = 0x0004;
const uint16_t CONFIG_CONV_TIME_2116US = 0x0005;
const uint16_t CONFIG_CONV_TIME_4156US = 0x0006;
const uint16_t CONFIG_CONV_TIME_8244US = 0x0007;
ValueDescriptionU16 convTimeSel[] = {
	{ CONFIG_CONV_TIME_140US, "140 us" },
	{ CONFIG_CONV_TIME_204US, "204 us" },
	{ CONFIG_CONV_TIME_332US, "332 us" },
	{ CONFIG_CONV_TIME_588US, "588 us" },
	{ CONFIG_CONV_TIME_1100US, "1100 us" },
	{ CONFIG_CONV_TIME_2116US, "2116 us" },
	{ CONFIG_CONV_TIME_4156US, "4156 us" },
	{ CONFIG_CONV_TIME_8244US, "8244 us" },
};

const uint16_t CONFIG_MODE_SHUT_AND_BUS_CONTINUOUS = 0x0007;

/*
16-bit ADC
shunt voltage range: -81.9175...81.92mV, 2.5uV/LSB
bus voltage range: 0...36V, 1.25mV/LSB
*/
const double SHUNT_VOLTAGE_UNIT = 2.5e-6;
const double VBUS_VOLTAGE_UNIT = 1.25e-3;


}


__fastcall TfrmCH341Ina226::TfrmCH341Ina226(TComponent* Owner)
	: TForm(Owner),
	currentLsb(0)
{
	TabManager::Instance().Register(this);
	cbAddress->Clear();
	for (unsigned int i=0; i<I2C_ADDRESS_COUNT; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X", I2C_START_ADDRESS + i);
		cbAddress->Items->Add(text);
	}
	cbAddress->ItemIndex = 0;

	FillComboboxWithValues(avgSel, cbAvgSel, CONFIG_AVG_1);
	FillComboboxWithValues(convTimeSel, cbVbusConversionSel, CONFIG_CONV_TIME_1100US);
	FillComboboxWithValues(convTimeSel, cbShuntConversionSel, CONFIG_CONV_TIME_1100US);
}
//---------------------------------------------------------------------------

uint8_t TfrmCH341Ina226::GetAddress(void)
{
	return static_cast<uint8_t>(I2C_START_ADDRESS + cbAddress->ItemIndex);
}

void __fastcall TfrmCH341Ina226::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();
	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	double shuntResistance, maxExpectedCurrent;
	if (sscanf(edShuntResistance->Text.c_str(), "%lf", &shuntResistance) != 1)
	{
		lblStatus->Caption = "Invalid value entered as shunt resistance!";
		return;
	}
	if (sscanf(edShuntMaxExpectedCurrent->Text.c_str(), "%lf", &maxExpectedCurrent) != 1)
	{
		lblStatus->Caption = "Invalid value entered as max expected current!";
		return;
	}

	uint16_t config =
		CONFIG_MODE_SHUT_AND_BUS_CONTINUOUS |
		static_cast<uint16_t>(avgSel[cbAvgSel->ItemIndex].value << CONFIG_AVG_OFFSET) |
		static_cast<uint16_t>(convTimeSel[cbVbusConversionSel->ItemIndex].value << CONFIG_VBUS_CONV_TIME_OFFSET) |
		static_cast<uint16_t>(convTimeSel[cbShuntConversionSel->ItemIndex].value << CONFIG_SHUNT_CONV_TIME_OFFSET);
	WriteReg(REG_CONFIG, config);

	currentLsb = maxExpectedCurrent / (1<<15);
	uint16_t cal = static_cast<uint16_t>(0.00512 / (currentLsb * shuntResistance));
	WriteReg(REG_CALIBRATION, cal);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Ina226::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341Ina226::Read(void)
{
	BtnController btnCtrl(btnRead);


	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	AnsiString text;
	
	int16_t vbusReg = ReadReg(REG_BUS_VOLTAGE);
	double vbusVoltage = VBUS_VOLTAGE_UNIT * vbusReg;
	edVbusReg->Text = vbusReg;
	text.sprintf("%.3f", vbusVoltage);
	edVbusVoltage->Text = text;

	int16_t shuntReg = ReadReg(REG_SHUNT_VOLTAGE);
	double shuntVoltage = SHUNT_VOLTAGE_UNIT * shuntReg;
	edShuntReg->Text = shuntReg;
	text.sprintf("%.3f", shuntVoltage * 1000.0);
	edShuntVoltage->Text = text;

	double current = currentLsb * shuntReg;
	text.sprintf("%.3f", current);
	edShuntCurrent->Text = text;

	int16_t powerReg = ReadReg(REG_POWER);
	edPowerReg->Text = powerReg;
	text.sprintf("%.3f", currentLsb * 25 * powerReg);
	edPower->Text = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341Ina226::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Ina226::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Ina226::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

int TfrmCH341Ina226::WriteReg(uint8_t reg, uint16_t val)
{
	uint8_t address = GetAddress();
	uint8_t buf[3];
	buf[0] = reg;
	buf[1] = static_cast<uint8_t>(val >> 8);
	buf[2] = static_cast<uint8_t>(val & 0xFF);
	return ch341a.I2CWriteBytes(address, buf, sizeof(buf));
}

int16_t TfrmCH341Ina226::ReadReg(uint8_t reg)
{
	uint8_t address = GetAddress();
	uint8_t buf[2] = { 0xFF, 0xFF };
	ch341a.I2CWriteCommandReadBytes(address, reg, buf, sizeof(buf));
	int16_t ret = 0;;
	uint8_t *ptr = reinterpret_cast<uint8_t*>(&ret);
	ptr[1] = buf[0];
	ptr[0] = buf[1];
	return ret;
}

double TfrmCH341Ina226::ReadVbusVoltage(void)
{
	int16_t reg = ReadReg(REG_BUS_VOLTAGE);
	return VBUS_VOLTAGE_UNIT * reg;
}

double TfrmCH341Ina226::ReadShuntVoltage(void)
{
	int16_t reg = ReadReg(REG_SHUNT_VOLTAGE);
	return SHUNT_VOLTAGE_UNIT * reg;
}


