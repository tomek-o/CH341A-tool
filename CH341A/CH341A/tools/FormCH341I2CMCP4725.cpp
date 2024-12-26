//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CMCP4725.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CMCP4725 *frmCH341I2CMCP4725;
//---------------------------------------------------------------------------

namespace
{

enum { MCP4725_ADDR_START = 0x60 };

enum { MCP4725_GENERAL_CALL_ADDRESS = 0x00 };
enum { MCP4725_GENERAL_CALL_CMD_RESET = 0x06 };
enum { MCP4725_GENERAL_CALL_CMD_WAKEUP = 0x09 };

}

__fastcall TfrmCH341I2CMCP4725::TfrmCH341I2CMCP4725(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
	edValueChange(NULL);
}
//---------------------------------------------------------------------------

uint8_t TfrmCH341I2CMCP4725::GetAddress(void)
{
	return static_cast<uint8_t>(MCP4725_ADDR_START + cbAddress->ItemIndex);
}

void TfrmCH341I2CMCP4725::Read(void)
{
	BtnController btnCtrl(btnRead);


}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341I2CMCP4725::btnWriteDacFastModeClick(TObject *Sender)
{
	BtnController btnCtrl(btnWriteDacFastMode);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();

	uint8_t data[2];
	int val = StrToIntDef(edValue->Text, -1);
	if (val < 0 || val >= 4096)
	{
		lblStatus->Caption = "Value out of range!";
		return;
	}
	data[0] = static_cast<uint8_t>(val / 256);
	data[1] = static_cast<uint8_t>(val % 256);

	int status = ch341a.I2CWriteBytes(address, data, sizeof(data));
	if (status != 0)
	{
		lblStatus->Caption = "Failed to write value";
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMCP4725::edValueChange(TObject *Sender)
{
	int val = StrToIntDef(edValue->Text, -1);
	if (val < 0 || val >= 4096)
	{
		lblStatus->Caption = "Value out of range!";
		lblVal->Caption = "";
		return;
	}
	double val3 = 3.3 * val / 4096;
	double val5 = 5.0 * val / 4096;
	AnsiString text;
	text.sprintf("(%.3f V at 3.3 V Vdd, %.3f V at 5 V Vdd)", val3, val5);
	lblVal->Caption = text;
	lblStatus->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMCP4725::btnWriteDacAndEepromClick(TObject *Sender)
{
	BtnController btnCtrl(btnWriteDacAndEeprom);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();

	uint8_t data[3];
	int val = StrToIntDef(edValue->Text, -1);
	if (val < 0 || val >= 4096)
	{
		lblStatus->Caption = "Value out of range!";
		return;
	}
	data[0] = 0x60;
	data[1] = static_cast<uint8_t>(val / 16);
	data[2] = static_cast<uint8_t>((val % 16) << 4);

	int status = ch341a.I2CWriteBytes(address, data, sizeof(data));
	if (status != 0)
	{
		lblStatus->Caption = "Failed to write value";
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMCP4725::btnReadClick(TObject *Sender)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		lblRead->Caption = "";
		return;
	}

	uint8_t address = GetAddress();

	uint8_t buffer[5];

	int status = ch341a.I2CReadBytes(address, buffer, sizeof(buffer));
	if (status != 0)
	{
		lblStatus->Caption = "Failed to read buffer";
		lblRead->Caption = "";
		return;
	}

	unsigned int dac = (buffer[1] * 16) + (buffer[2] >> 4);
	unsigned int eeprom = ((buffer[3] & 0x0F) * 256) + buffer[4];
	unsigned int powerDown = (buffer[3] & 0x60) >> 5;
	AnsiString text;
	text.sprintf("DAC = %u, EEPROM = %u, power down = %u", dac, eeprom, powerDown);
	lblRead->Caption = text;
}
//---------------------------------------------------------------------------

