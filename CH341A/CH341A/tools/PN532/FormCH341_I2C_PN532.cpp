//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341_I2C_PN532.h"
#include "PN532.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "common/bin2str.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341_I2C_PN532 *frmCH341_I2C_PN532;
//---------------------------------------------------------------------------


__fastcall TfrmCH341_I2C_PN532::TfrmCH341_I2C_PN532(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_PN532::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	lblStatus->Caption = "";
	
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(PN532_I2C_ADDRESS);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	pn532.begin();

	uint32_t versiondata = pn532.getFirmwareVersion();
	if (! versiondata) {
		lblStatus->Caption = "Failed to get PN532 firmware version!";
		return;
	}

	// Got ok data, print it out!
	version.sprintf("Found PN5%02X , version %d.%d",
		(versiondata>>24) & 0xFF,
		(versiondata>>16) & 0xFF,
		(versiondata>>8) & 0xFF
	);

	// Set the max number of retry attempts to read from a card
	// This prevents us from waiting forever for a card, which is
	// the default behaviour of the PN532.
	pn532.setPassiveActivationRetries(0xFF);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_PN532::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341_I2C_PN532::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(PN532_I2C_ADDRESS);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	bool success;
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
	uint8_t uidLength;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type)

	// Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
	// 'uid' will be populated with the UID, and uidLength will indicate
	// if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
	success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

	if (success)
	{
		AnsiString text;
		text.cat_printf("Found card:\n");
		text.cat_printf("UID Length: %d B\n", uidLength);
		text.cat_printf("UID Value: %s", BufToSpaceSeparatedHexString(uid, uidLength).c_str());
	}
	else
	{
		// PN532 probably timed out waiting for a card
		memo->Text = "Timed out waiting for a card";
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341_I2C_PN532::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_PN532::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_PN532::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

