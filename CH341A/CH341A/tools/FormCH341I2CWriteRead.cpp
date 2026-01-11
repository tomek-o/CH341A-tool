//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CWriteRead.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/bin2str.h"
#include <vector>
#include <stdint.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CWriteRead *frmCH341I2CWriteRead;

//---------------------------------------------------------------------------
__fastcall TfrmCH341I2CWriteRead::TfrmCH341I2CWriteRead(TComponent* Owner)
	: TForm(Owner)
{
	for (unsigned int i=0; i<128; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbI2CAddress->Items->Add(text);
	}
	cbI2CAddress->ItemIndex = 80;	// 24C02 address as default device address

	for (unsigned int i=0; i<=/* CH341A::GetMaxDataLengthInRequest()*/ 1024; i++)
	{
		cbReadBytesCount->Items->Add(i);
	}
	cbReadBytesCount->ItemIndex = 0;

	TabManager::Instance().Register(this, 1u << ToolGroupGenericInterfaces);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CWriteRead::btnWriteReadClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	memoRead->Clear();
	unsigned int address = cbI2CAddress->ItemIndex;

	int status;
	AnsiString msg;
	std::vector<uint8_t> writeData;

	status = HexStringCleanToBuf(memoWrite->Text, msg, writeData);
	if (status != 0)
	{
		MessageBox(this->Handle, msg.c_str(), "Failed to convert text to data to write", MB_ICONEXCLAMATION);
		return;
	}

	writeData.insert(writeData.begin(), static_cast<uint8_t>(address << 1));

	unsigned int bytesToRead = cbReadBytesCount->ItemIndex;
	std::vector<uint8_t> readData;
	readData.resize(bytesToRead);

	status = ch341a.I2CWriteRead(&writeData[0], writeData.size(), !readData.empty() ? &readData[0] : NULL, readData.size());
	if (status != 0)
	{
		MessageBox(this->Handle, "Error writing/reading I2C", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}


#if 0
	int count = StrToIntDef(cbReadBytesCount->Text, -1);
	if (count < 0)
	{
		MessageBox(this->Handle, "Invalid number of bytes to read", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	std::vector<uint8_t> data;
	data.resize(count);
	int status = ch341a.I2CRead(&data[0], count);
	if (status != 0)
	{
		MessageBox(this->Handle, "Read error", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
#endif

	AnsiString text;
	for (unsigned int i=0; i<readData.size(); i++)
	{
		text.cat_printf("0x%02X", readData[i]);
		if ((i % 7) == 7)
			text.cat_printf("\n");
		else
			text.cat_printf(" ");
	}
	memoRead->Text = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CWriteRead::memoWriteChange(TObject *Sender)
{
	int status;
	AnsiString msg;
	std::vector<uint8_t> writeData;

	status = HexStringCleanToBuf(memoWrite->Text, msg, writeData);
	(void)status;

	AnsiString text;
	text.sprintf("%u byte(s)", writeData.size());
	lblWriteBytesCount->Caption = text;
}
//---------------------------------------------------------------------------

