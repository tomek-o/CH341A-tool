//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341Template.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341Template *frmCH341Template;
//---------------------------------------------------------------------------

#define L3G4200D_ADDRESS_A			0x68
#define L3G4200D_ADDRESS_B			0x69



__fastcall TfrmCH341Template::TfrmCH341Template(TComponent* Owner)
	: TForm(Owner),
	dpsPerBit(0)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------

uint8_t TfrmCH341Template::GetAddress(void)
{
	switch (cbAddress->ItemIndex)
	{
	case 0:
		return L3G4200D_ADDRESS_A;
	case 1:
		return L3G4200D_ADDRESS_B;
	default:
		assert(!"Unhandled address!");
		return 0;
	}
}

void __fastcall TfrmCH341Template::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();


}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Template::btnReadClick(TObject *Sender)
{
	Read();
}

AnsiString TfrmCH341Template::RawValToString(int val)
{
	AnsiString text = FloatToStrF(dpsPerBit * val, ffFixed, 6, 1);
	if (val >= 0)
		text = (AnsiString)" " + text;
	return text;
}

void TfrmCH341Template::Read(void)
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
void __fastcall TfrmCH341Template::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------





