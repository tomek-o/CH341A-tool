//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CSmartBatteryInfo.h"
#include "CH341A.h"
#include "TabManager.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CSmartBatteryInfo *frmCH341I2CSmartBatteryInfo;
//---------------------------------------------------------------------------
__fastcall TfrmCH341I2CSmartBatteryInfo::TfrmCH341I2CSmartBatteryInfo(TComponent* Owner)
	: TForm(Owner)
{
	for (unsigned int i=0; i<128; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbI2CAddress->Items->Add(text);
	}
	cbI2CAddress->ItemIndex = 11;

	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CSmartBatteryInfo::btnReadInfoClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	memoInfo->Clear();
	unsigned int address = cbI2CAddress->ItemIndex;

	
}
//---------------------------------------------------------------------------
