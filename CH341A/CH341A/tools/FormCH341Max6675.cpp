//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341Max6675.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341Max6675 *frmCH341Max6675;
//---------------------------------------------------------------------------


__fastcall TfrmCH341Max6675::TfrmCH341Max6675(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Max6675::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341Max6675::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t buf[2] = {0x00, 0x00};
	int status = ch341a.SpiTransfer(buf, sizeof(buf));
	if (status)
	{
		lblStatus->Caption = "SPI transfer failed!";
		return;
	}

	if (buf[1] & 0x04)
	{
		lblStatus->Caption = "No thermocouple attached to MAX6675!";
		return;
	}

	uint16_t raw = static_cast<uint16_t>(((uint16_t)buf[0] << 8) | buf[1]);
	double temperature = static_cast<double>(raw >> 3) * 0.25;
	AnsiString tempText = FloatToStrF(temperature, ffFixed, 6, 2);
	AnsiString text;
	text.sprintf("Temperature: %s Celsius degrees", tempText.c_str());
	lblStatus->Caption = text;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCH341Max6675::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
	{
		Read();
		tmrAutoRead->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Max6675::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Max6675::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

