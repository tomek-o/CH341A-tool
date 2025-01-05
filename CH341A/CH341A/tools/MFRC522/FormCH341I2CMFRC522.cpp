//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CMFRC522.h"
#include "MFRC522_I2C.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CMFRC522 *frmCH341I2CMFRC522;
//---------------------------------------------------------------------------

namespace
{

MFRC522 mfrc522(0x24);

}

__fastcall TfrmCH341I2CMFRC522::TfrmCH341I2CMFRC522(TComponent* Owner)
	: TForm(Owner),
	dpsPerBit(0)
{
	TabManager::Instance().Register(this);

	for (unsigned int i=0; i<64; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbAddress->Items->Add(text);
	}
	cbAddress->ItemIndex = 0x24;

}
//---------------------------------------------------------------------------

uint8_t TfrmCH341I2CMFRC522::GetAddress(void)
{
	return static_cast<uint8_t>(cbAddress->ItemIndex);
}

void __fastcall TfrmCH341I2CMFRC522::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	memo->Clear();

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}	

	mfrc522.PCD_Init(address);
	ShowReaderDetails();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMFRC522::btnReadClick(TObject *Sender)
{
	Read();
}

AnsiString TfrmCH341I2CMFRC522::RawValToString(int val)
{
	AnsiString text = FloatToStrF(dpsPerBit * val, ffFixed, 6, 1);
	if (val >= 0)
		text = (AnsiString)" " + text;
	return text;
}

void TfrmCH341I2CMFRC522::Read(void)
{
	BtnController btnCtrl(btnRead);


	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}


}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CMFRC522::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void TfrmCH341I2CMFRC522::ShowReaderDetails(void)
{
	AnsiString text;
	// Get the MFRC522 software version
	byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
	text.cat_printf("MFRC522 Software Version: 0x%02X", v);
	if (v == 0x91)
		text.cat_printf(" = v1.0");
	else if (v == 0x92)
		text.cat_printf(" = v2.0");
	else
		text.cat_printf(" (unknown)");
	text.cat_printf("\r\n");
	// When 0x00 or 0xFF is returned, communication probably failed
	if ((v == 0x00) || (v == 0xFF)) {
		text.cat_printf("WARNING: Communication failure, is the MFRC522 properly connected?");
	}

	memo->Text = text;	
}



