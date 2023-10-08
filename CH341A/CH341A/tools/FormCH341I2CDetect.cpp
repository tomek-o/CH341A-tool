//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CDetect.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CDetect *frmCH341I2CDetect;
//---------------------------------------------------------------------------
__fastcall TfrmCH341I2CDetect::TfrmCH341I2CDetect(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CDetect::btnDetectDevicesClick(TObject *Sender)
{
	Detect();
}

void TfrmCH341I2CDetect::Detect(void)
{
	BtnController btnCtrl(btnDetectDevices);
	if (!ch341a.IsOpened())
	{
		memoDetect->Text = "CH341 is not opened!";
		return;
	}
	memoDetect->Clear();
	AnsiString text;
	bool found = false;
	for (uint8_t i=0; i<=127; i++)
	{
		if (ch341a.I2CCheckDev(i) == 0)
		{
			text.cat_printf("Found device at address %u (0x%02X)\n", static_cast<unsigned int>(i), static_cast<unsigned int>(i));
			found = true;
		}
	}
	if (found)
	{
		memoDetect->Text = text;
	}
	else
	{
    	memoDetect->Text = "No I2C device found (no I2C ACK on any address)!";
	}	
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CDetect::tmrAutoDetectTimer(TObject *Sender)
{
	if (chbAutoDetect->Checked)
		Detect();	
}
//---------------------------------------------------------------------------

