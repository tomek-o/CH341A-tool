//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SpiWriteRead.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/bin2str.h"
#include <vector>
#include <stdint.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SpiWriteRead *frmCH341SpiWriteRead;

//---------------------------------------------------------------------------
__fastcall TfrmCH341SpiWriteRead::TfrmCH341SpiWriteRead(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SpiWriteRead::btnWriteReadClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	memoRead->Clear();

	int status;
	AnsiString msg;
	std::vector<uint8_t> rwData;

	status = HexStringCleanToBuf(memoWrite->Text, msg, rwData);
	if (status != 0)
	{
		MessageBox(this->Handle, msg.c_str(), "Failed to convert text to data to write", MB_ICONEXCLAMATION);
		return;
	}

	if (rwData.size() == 0)
	{
    	return;
	}

	status = ch341a.SpiTransfer(&rwData[0], rwData.size());
	if (status != 0)
	{
		MessageBox(this->Handle, "Error writing/reading SPI", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	AnsiString text;
	for (unsigned int i=0; i<rwData.size(); i++)
	{
		text.cat_printf("0x%02X", rwData[i]);
		if ((i % 7) == 7)
			text.cat_printf("\n");
		else
			text.cat_printf(" ");
	}
	memoRead->Text = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SpiWriteRead::memoWriteChange(TObject *Sender)
{
	int status;
	AnsiString msg;
	std::vector<uint8_t> rwData;

	status = HexStringCleanToBuf(memoWrite->Text, msg, rwData);
	(void)status;

	AnsiString text;
	text.sprintf("%u byte(s)", rwData.size());
	lblWriteBytesCount->Caption = text;
}
//---------------------------------------------------------------------------

