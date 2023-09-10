//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341A.h"
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfrmCH341A::TfrmCH341A(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341A::btnOpenClick(TObject *Sender)
{
	const int devIndex = 0;

	int status = ch341a.Open(devIndex);
	if (status != 0)
	{
		MessageBox(this->Handle, "Could not open CH341A device", this->Caption.c_str(), MB_ICONEXCLAMATION);
	}	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341A::btnCloseClick(TObject *Sender)
{
	ch341a.Close();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341A::btnReadClick(TObject *Sender)
{
	int count = StrToIntDef(cbReadBytesCount->Text, -1);
	if (count < 0)
	{
		MessageBox(this->Handle, "Invalid number of bytes to read", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	memoRead->Clear();

	std::vector<uint8_t> data;
	data.resize(count);
	int status = ch341a.I2CRead(&data[0], count);
	if (status != 0)
	{
		MessageBox(this->Handle, "Read error", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	AnsiString text;
	for (unsigned int i=0; i<data.size(); i++)
	{
		text.cat_printf("0x%02X", data[i]);
		if ((i % 7) == 7)
			text.cat_printf("\n");
		else
			text.cat_printf(" ");
	}
	memoRead->Text = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341A::btnDetectDevicesClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
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
