//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341_SPI_PN532.h"
#include "PN532.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/ScopedBool.h"
#include "common/bin2str.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341_SPI_PN532 *frmCH341_SPI_PN532;
//---------------------------------------------------------------------------


__fastcall TfrmCH341_SPI_PN532::TfrmCH341_SPI_PN532(TComponent* Owner)
	: TForm(Owner),
	pn532(PN532::INTERFACE_SPI),
	reading(false),
	busy(false)
{
	TabManager::Instance().Register(this, 1u << ToolGroupMisc);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_SPI_PN532::btnStartClick(TObject *Sender)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	lblStatus->Caption = "";

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	LOG("PN532 (SPI): begin\n");
	int status = pn532.begin();
	if (status != 0)
	{
		lblStatus->Caption = "Failed to init / wake up PN532";
		return;
	}

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
	lblStatus->Caption = version;

	// Limit passive activation retries so InListPassiveTarget completes by
	// itself (reporting 0 targets) when no card is present. 0xFF would mean
	// retry forever - the chip then stays busy until the command is aborted.
	pn532.setPassiveActivationRetries(0x10);

	reading = true;
	lblReadingState->Caption = "Reading...";
	btnStart->Enabled = false;
	btnStop->Enabled = true;
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_SPI_PN532::btnStopClick(TObject *Sender)
{
	reading = false;
	tmrAutoRead->Enabled = false;
	lblReadingState->Caption = "Stopped";
	btnStop->Enabled = false;
	btnStart->Enabled = true;
}
//---------------------------------------------------------------------------

void TfrmCH341_SPI_PN532::Read(void)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	bool success;
	uint8_t uid[PN532_UID_MAX_LEN] = { 0 };	// Buffer to store the returned UID
	uint8_t uidLength;				// Length of the UID (4, 7 or 10 bytes depending on ISO14443A card type)

	// Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
	// 'uid' will be populated with the UID, and uidLength will indicate
	// if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
	success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 1000);

	if (success)
	{
		AnsiString text;
		text.cat_printf("Found card:\n");
		text.cat_printf("UID Length: %d B\n", uidLength);
		text.cat_printf("UID Value: %s", BufToSpaceSeparatedHexString(uid, uidLength).c_str());
		memo->Lines->Text = text;	///< \note if assigning to memo->Text \n newlines would not be properly interpreted
	}
	else
	{
		memo->Lines->Text = "No card found";
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_SPI_PN532::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (reading)
		Read();
	// Stop may have been clicked while Read() was in progress
	if (reading)
		tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

