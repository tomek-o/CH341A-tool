//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMPR121.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMPR121 *frmMPR121;
//---------------------------------------------------------------------------


__fastcall TfrmMPR121::TfrmMPR121(TComponent* Owner)
	: TForm(Owner),
	proximityEnable(false)
{
	TabManager::Instance().Register(this, (1u<<ToolGroupKeypad));
	for (int i=0; i<=255; i++)
	{
		cbTouchThreshold->Items->Add(i);
		cbReleaseThreshold->Items->Add(i);
	}
	cbTouchThreshold->ItemIndex = 15;
	cbReleaseThreshold->ItemIndex = 10;
}
//---------------------------------------------------------------------------

uint8_t TfrmMPR121::GetAddress(void)
{
	return static_cast<uint8_t>(MPR121_I2CADDR_DEFAULT + cbAddress->ItemIndex);
}

void __fastcall TfrmMPR121::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();
	proximityEnable = chbProximityChannel->Checked;

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	mpr121Inst.setAddress(address);

    // set autoconfig charge level based on 3.2V
    // without this, it will assume 1.8V (a safe default, but not always ideal)
	mpr121Inst.autoConfigUSL = 256L * (3200 - 700) / 3200;

    // enable proximity sensing if it's set to true
    if (proximityEnable)
	  mpr121Inst.proxEnable = MPR_ELEPROX_0_TO_11;
    else
	  mpr121Inst.proxEnable = MPR_ELEPROX_DISABLED;

    // optional: change touch/release thresholds
    // note: these are only used for digital touch state reads
	mpr121Inst.setAllThresholds(
		static_cast<uint8_t>(cbTouchThreshold->ItemIndex),
		static_cast<uint8_t>(cbReleaseThreshold->ItemIndex),
		true);

    // start sensing (for 12 electrodes)
	mpr121Inst.start(12);

	lblStatus->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmMPR121::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmMPR121::Read(void)
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

	enum { REGULAR_CHANNELS = 12 };

	// readTouchState only reads the digital state, not full analog data
	// it returns a short with individual bits reflecting the electrodes
	// the bits can be read using bitRead
	bool* touches = mpr121Inst.readTouchState();

	AnsiString text = "Touch state, from LSB: ";

	for (int j = 0; j < REGULAR_CHANNELS; j++) {
	if (touches[j])
		text += "1 ";
	else
		text += "0 ";
	}

	// readElectrodeData is used to read analog values
	// it returns a pointer to a short
	// in this usage, a pointer works just like an array would and can be accessed like values[j]
	short* rawValues = mpr121Inst.readElectrodeData(0, REGULAR_CHANNELS + 1); // read all electrodes, starting from electrode 0
	text.cat_printf("\nRaw electrode data (from LSB):\n    ");
	for (int i=0; i < REGULAR_CHANNELS; i++)
	{
		text.cat_printf(" %4d", rawValues[i]);
	}

	if (proximityEnable) {
		text.cat_printf("\nProximity channel: %d, raw data: %d",
			touches[REGULAR_CHANNELS] ? 1 : 0,
			rawValues[REGULAR_CHANNELS]
			);
	}

	lblStatus->Caption = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMPR121::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
	{
		Read();
		tmrAutoRead->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMPR121::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMPR121::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

