//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CSi5351.h"
#include "CH341A.h"
#include "SI5351.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <limits.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CSi5351 *frmCH341I2CSi5351;
//---------------------------------------------------------------------------


__fastcall TfrmCH341I2CSi5351::TfrmCH341I2CSi5351(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341I2CSi5351::btnWriteClick(TObject *Sender)
{
	BtnController btnCtrl(btnWrite);

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(SI5351_ADDRESS);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected Si5351 address!";
		return;
	}

	const int freqMin = 2500;
	const int freqMax = 200000000;

	int fA = StrToIntDef(edFrequencyA->Text, -1);
	if (fA < freqMin || fA > freqMax)
	{
		AnsiString msg;
		msg.sprintf("Invalid frequency specified for PLL A, expected %d ... %d Hz!", freqMin, freqMax);
		lblStatus->Caption = msg;
		return;
	}

	int fB = StrToIntDef(edFrequencyB->Text, -1);
	if (fB < freqMin || fB > freqMax)
	{
		AnsiString msg;
		msg.sprintf("Invalid frequency specified for PLL B, expected %d ... %d Hz!", freqMin, freqMax);
		lblStatus->Caption = msg;
		return;
	}

	int correction = StrToIntDef(edCorrection->Text, INT_MIN);
	if (correction == INT_MIN)
	{
		lblStatus->Caption = "Invalid frequency correction value!";
		return;
	}

	si5351_Init(correction);
	si5351_SetupCLK0(fA, static_cast<si5351DriveStrength_t>(cbDriveStrength0->ItemIndex));
	si5351_SetupCLK2(fB, static_cast<si5351DriveStrength_t>(cbDriveStrength2->ItemIndex));
	si5351_EnableOutputs((1 << 2) | (1 << 0));
}
//---------------------------------------------------------------------------

