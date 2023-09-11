//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CReadWrite.h"
#include "CH341A.h"
#include "TabManager.h"
#include <vector>
#include <stdint.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CReadWrite *frmCH341I2CReadWrite;
//---------------------------------------------------------------------------
__fastcall TfrmCH341I2CReadWrite::TfrmCH341I2CReadWrite(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CReadWrite::btnReadClick(TObject *Sender)
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
