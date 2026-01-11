//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341_I2C_VEML7700.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CVeml7700 *frmCH341I2CVeml7700;
//---------------------------------------------------------------------------


__fastcall TfrmCH341I2CVeml7700::TfrmCH341I2CVeml7700(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341I2CVeml7700::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CVeml7700::Read(void)
{
	BtnController btnCtrl(btnRead);


	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = VEML7700::I2C_ADDRESS;

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	veml7700.begin();

	float lux = -1;
	veml7700.getAutoALSLux(lux);   // Get the measured ambient light value

	float luxWhite = -1;
	veml7700.getAutoWhiteLux(luxWhite);

	AnsiString text;
	text.sprintf(
		"Ambient light (human eye-like sensitivity filter): %.2f lux\r\n"
		"White/wide spectrum channel (no filter): %.2f units"
		,
		lux, luxWhite);
	lblStatus->Caption = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CVeml7700::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CVeml7700::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CVeml7700::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

