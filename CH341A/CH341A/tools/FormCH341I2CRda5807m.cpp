//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CRda5807m.h"
#include "CH341A.h"
#include "RDA5807M/RDA5807M.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CRda5807m *frmCH341I2CRda5807m;
//---------------------------------------------------------------------------

__fastcall TfrmCH341I2CRda5807m::TfrmCH341I2CRda5807m(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CRda5807m::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CRda5807m::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";

	struct RDA5807M_status status;
	memset(&status, 0, sizeof(status));	

	RDA5807M_get_status(&status);

	AnsiString text;

	text.sprintf("STATUS: %.02f MHz, RSSI %u, %s, %s, %s",
		static_cast<float>(status.frequency) / 100.0f,
		status.rssi,
		status.tuneok ? "TUNED" : "TUNING",
		status.tunefail ? "SEEK FAILED" : "SEEK OK",
		status.stereo ? "STEREO" : "MONO"
	);

	lblRdaStatus->Caption = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CRda5807m::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CRda5807m::btnInitClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";
	
	RDA5807M_init();
	RDA5807M_set_volume(static_cast<uint8_t>(trbarVolume->Position));
	Read();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CRda5807m::btnSearchUpClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";
	
	RDA5807M_search(1);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CRda5807m::btnSearchDownClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";
	
	RDA5807M_search(0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CRda5807m::trbarVolumeChange(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";	

	RDA5807M_set_volume(static_cast<uint8_t>(trbarVolume->Position));
}
//---------------------------------------------------------------------------

