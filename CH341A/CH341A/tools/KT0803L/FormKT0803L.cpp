//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormKT0803L.h"
#include "kt0803l.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmKT0803L *frmKT0803L;
//---------------------------------------------------------------------------

__fastcall TfrmKT0803L::TfrmKT0803L(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, (1u<<ToolGroupFmRadio));
}
//---------------------------------------------------------------------------

void __fastcall TfrmKT0803L::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(KT0803L_ADDR);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
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

	KT0803L_SETTING txSetting;
	memset(&txSetting, 0, sizeof(txSetting));
	txSetting.useExtInductor=KT0803L_FALSE;
	txSetting.clkSetting.isUpToSW=KT0803L_TRUE;
	txSetting.clkSetting.isXTAL=KT0803L_TRUE;
	txSetting.clkSetting.freq=KT0803L_CLK_FREQ_32_768KHZ;

	txSetting.isPLTAmpHigh = static_cast<KT0803L_BOOL>(chbHighPilotAmplitude->Checked ? KT0803L_TRUE : KT0803L_FALSE);
	txSetting.isPHTCNST50us = static_cast<KT0803L_BOOL>((cbPreemphasis->ItemIndex == 1) ? KT0803L_TRUE : KT0803L_FALSE);
	txSetting.isFDEV112_5KHZ=KT0803L_FALSE;
	txSetting.isCHSELPAOff=KT0803L_FALSE;

	if (KT0803L_init(&txSetting) != KT0803L_OK)
	{
		lblStatus->Caption = "KT0803L init failed";
		return;
	}
	KT0803L_setFreq(static_cast<unsigned short>(frequency * 10 + 0.5));	// 960 -> 96.0 MHz
	KT0803L_setPGAGain(KT0803L_PGA_GAIN_M5DB, KT0803L_FALSE);
	KT0803L_PADown(KT0803L_FALSE);

	lblStatus->Caption = "";
}
//---------------------------------------------------------------------------


