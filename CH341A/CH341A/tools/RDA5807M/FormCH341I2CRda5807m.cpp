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

namespace
{
	// RDS reception needs frequent polling to catch enough groups to assemble PS/RadioText;
	// without it, a slow once-a-second poll is plenty for just the tuner status.
	enum { AUTO_READ_INTERVAL_MS_RDS = 30, AUTO_READ_INTERVAL_MS_NO_RDS = 1000 };
}

__fastcall TfrmCH341I2CRda5807m::TfrmCH341I2CRda5807m(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupFmRadio);
	UpdateRdsUiState();
}

void TfrmCH341I2CRda5807m::UpdateRdsUiState(void)
{
	bool enabled = chbRdsEnabled->Checked;

	edRdsPi->Visible = enabled;
	edRdsPs->Visible = enabled;
	edRdsFlags->Visible = enabled;
	edRdsRt->Visible = enabled;
	lblRdsPi->Visible = enabled;
	lblRdsPs->Visible = enabled;
	lblRdsRt->Visible = enabled;
	lblRdsFlags->Visible = enabled;

	tmrAutoRead->Interval = enabled ? AUTO_READ_INTERVAL_MS_RDS : AUTO_READ_INTERVAL_MS_NO_RDS;
}

void TfrmCH341I2CRda5807m::ClearRdsDisplay(void)
{
	edRdsPi->Text = "";
	edRdsPs->Text = "";
	edRdsFlags->Text = "";
	edRdsRt->Text = "";
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

	if (chbRdsEnabled->Checked)
	{
		struct RDA5807M_rds_status rds;
		memset(&rds, 0, sizeof(rds));
		RDA5807M_get_rds(&rds);

		AnsiString piText;
		piText.sprintf("%04X", static_cast<unsigned int>(rds.piCode));
		edRdsPi->Text = rds.valid ? piText : AnsiString("");

		edRdsPs->Text = rds.psReady ? AnsiString(rds.programService) : AnsiString("");
		edRdsRt->Text = rds.rtReady ? AnsiString(rds.radioText) : AnsiString("");

		AnsiString flagsText;
		flagsText.sprintf("PTY %u, %s%s", static_cast<unsigned int>(rds.programType),
			rds.trafficProgram ? "TP" : "tp",
			rds.trafficAnnouncement ? " TA" : "");
		edRdsFlags->Text = rds.valid ? flagsText : AnsiString("");
	}
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
	RDA5807M_set_rds_enabled(chbRdsEnabled->Checked);
	ClearRdsDisplay();
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
	ClearRdsDisplay();
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
	ClearRdsDisplay();
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
void __fastcall TfrmCH341I2CRda5807m::chbRdsEnabledClick(TObject *Sender)
{
	UpdateRdsUiState();

	if (!ch341a.IsOpened())
		return;

	RDA5807M_set_rds_enabled(chbRdsEnabled->Checked);
}
//---------------------------------------------------------------------------

