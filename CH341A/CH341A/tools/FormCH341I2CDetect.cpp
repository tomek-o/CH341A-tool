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
	TabManager::Instance().Register(this, 1u << ToolGroupGenericInterfaces);

	for (unsigned int i=0; i<128; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbI2CAddressStart->Items->Add(text);
		cbI2CAddressEnd->Items->Add(text);
	}
	cbI2CAddressStart->ItemIndex = 0;
	cbI2CAddressEnd->ItemIndex = cbI2CAddressEnd->Items->Count - 1;
	tmrAutoDetect->Interval = StrToInt(cbAutoDetectPeriod->Text);	
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

	const uint8_t START = static_cast<uint8_t>(cbI2CAddressStart->ItemIndex);
	const uint8_t END = static_cast<uint8_t>(cbI2CAddressEnd->ItemIndex);

	if (cbOutputFormatting->ItemIndex == 0)
	{
		for (uint8_t i=START; i<=END; i++)
		{
			if (ch341a.I2CCheckDev(i) == 0)
			{
				text.cat_printf("Found device at address %3u (0x%02X)\r\n", static_cast<unsigned int>(i), static_cast<unsigned int>(i));
				found = true;
			}
		}
		if (!found)
		{
			memoDetect->Text = "No I2C device found (no I2C ACK on any address)!";
		}
		else
		{
			memoDetect->Text = text;
		}
	}
	else
	{
		text.cat_printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");
		for (uint8_t i=0; i<=127; i++)
		{
			if ((i % 16) == 0)
			{
				text.cat_printf("\r\n%02x:", i);
			}
			if (i < START || i > END)
			{
				text.cat_printf(" ??");
			}
			else if (ch341a.I2CCheckDev(i) == 0)
			{
				text.cat_printf(" %02x", i);
				found = true;
			}
			else
			{
				text.cat_printf(" --");
			}
		}
		memoDetect->Text = text;		
	}
	if (found && chbBeep->Checked)
	{
			Beep(440, 100);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CDetect::tmrAutoDetectTimer(TObject *Sender)
{
	tmrAutoDetect->Enabled = false;
	if (chbAutoDetect->Checked)
		Detect();
	tmrAutoDetect->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CDetect::chbAutoDetectClick(TObject *Sender)
{
	tmrAutoDetect->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CDetect::chbAutoDetectMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoDetect->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CDetect::cbAutoDetectPeriodChange(TObject *Sender)
{
	tmrAutoDetect->Interval = StrToInt(cbAutoDetectPeriod->Text);	
}
//---------------------------------------------------------------------------

