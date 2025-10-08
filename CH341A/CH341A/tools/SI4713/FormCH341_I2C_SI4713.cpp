//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341_I2C_SI4713.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CSi4713 *frmCH341I2CSi4713;
//---------------------------------------------------------------------------

namespace
{

double round( double fValue )
{
	return fValue < 0 ? ceil( fValue - 0.5 )
		: floor( fValue + 0.5 );
}

}

__fastcall TfrmCH341I2CSi4713::TfrmCH341I2CSi4713(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------

uint8_t TfrmCH341I2CSi4713::GetAddress(void)
{
	switch (cbAddress->ItemIndex)
	{
	case 0:
		return SI4710_ADDR0;
	case 1:
		return SI4710_ADDR1;
	default:
		assert(!"Unhandled address!");
		return 0;
	}
}

void __fastcall TfrmCH341I2CSi4713::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();

	si4713.reset();	// not responding on I2C otherwise

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected Si4713 address!";
		//Clear();
		return;
	}

	if (!si4713.begin(GetAddress(), static_cast<Si4713::Preemphasis>(cbPreemphasis->ItemIndex))) {  // begin with address 0x63 (CS high default)
		lblStatus->Caption = "Couldn't init radio chip";
		return;
	}

	double frequency = 0.0;
	if (sscanf(edFrequency->Text.c_str(), "%lf", &frequency) != 1) {
		lblStatus->Caption = "Invalid frequency text entered!";
		return;
	}
	if (frequency < 76 || frequency > 108) {
		lblStatus->Caption = "Entered frequency is out of allowed range!";
		return;
	}

	unsigned int rdsProgramId = 0xC341;
	if (sscanf(edRdsProgramId->Text.c_str(), "%X", &rdsProgramId) != 1) {
		lblStatus->Caption = "Invalid RDS program ID entered!";
		return;
	}
	if (rdsProgramId > 0xFFFF) {
		lblStatus->Caption = "RDS program ID entered (out of range)!";
		return;
	}

	si4713.setTXpower(115);  // dBuV, 88-115 max
	si4713.tuneFM(static_cast<uint16_t>(round(frequency * 100.0)));
#if 0
	// This will tell you the status in case you want to read it from the chip
	radio.readTuneStatus();
	Serial.print("\tCurr freq: ");
	Serial.println(radio.currFreq);
	Serial.print("\tCurr freqdBuV:");
	Serial.println(radio.currdBuV);
	Serial.print("\tCurr ANTcap:");
	Serial.println(radio.currAntCap);
#endif
	// begin the RDS/RDBS transmission
	si4713.beginRDS(static_cast<uint16_t>(rdsProgramId));
	si4713.setRDSstation(edRdsStation->Text.c_str());
	si4713.setRDSbuffer(edRdsBuffer->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSi4713::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CSi4713::Read(void)
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

	si4713.readASQ();
	AnsiString text;
	text.sprintf("Current ASQ = 0x%02X, Input Audio Level = %ddB", si4713.currASQ, si4713.currInLevel);
	lblRadioStatus->Caption = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CSi4713::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSi4713::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSi4713::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

