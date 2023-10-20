//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSettings.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
__fastcall TfrmSettings::TfrmSettings(TComponent* Owner)
	: TForm(Owner)
{
	this->appSettings = NULL;
	pages->ActivePage = tsCH341A;

	cbI2CSpeed->Clear();
	for (unsigned int i=0; i<CH341AConf::I2C_SPEED__LIMITER; i++)
	{
		cbI2CSpeed->Items->Add(CH341AConf::getI2CSpeedDescription(static_cast<CH341AConf::I2CSpeed>(i)));
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::FormShow(TObject *Sender)
{
    assert(appSettings);
	tmpSettings = *appSettings;
	chbAlwaysOnTop->Checked = tmpSettings.frmMain.bAlwaysOnTop;

	chbLogToFile->Checked = tmpSettings.Logging.bLogToFile;
	cmbMaxUiLogLines->ItemIndex = -1;
	for (int i=0; i<cmbMaxUiLogLines->Items->Count; i++)
	{
		if ((unsigned int)StrToInt(cmbMaxUiLogLines->Items->Strings[i]) >= tmpSettings.Logging.iMaxUiLogLines)
		{
			cmbMaxUiLogLines->ItemIndex = i;
			break;
		}
	}
	if (cmbMaxUiLogLines->ItemIndex == -1)
	{
		cmbMaxUiLogLines->ItemHeight = cmbMaxUiLogLines->Items->Count - 1;
	}

	if (tmpSettings.ch341a.i2cSpeed < 0 || tmpSettings.ch341a.i2cSpeed >= cbI2CSpeed->Items->Count)
	{
		LOG("I2C speed index out of range!\n");
	}
	else
	{
		cbI2CSpeed->ItemIndex = tmpSettings.ch341a.i2cSpeed;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnCancelClick(TObject *Sender)
{
	this->Close();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnApplyClick(TObject *Sender)
{
	tmpSettings.Logging.bLogToFile = chbLogToFile->Checked;

	tmpSettings.ch341a.i2cSpeed = static_cast<CH341AConf::I2CSpeed>(cbI2CSpeed->ItemIndex);

	*appSettings = tmpSettings;
	this->Close();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbAlwaysOnTopClick(TObject *Sender)
{
	tmpSettings.frmMain.bAlwaysOnTop = chbAlwaysOnTop->Checked;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cmbMaxUiLogLinesChange(TObject *Sender)
{
	tmpSettings.Logging.iMaxUiLogLines = StrToInt(cmbMaxUiLogLines->Text);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::FormKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_ESCAPE)
	{
		Close();
	}
}
//---------------------------------------------------------------------------

