/** \brief Reading temperature from LM75 I2C sensor
*/

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CAs5600.h"
#include "AS5600/AS5600.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CAs5600 *frmCH341I2CAs5600;
//---------------------------------------------------------------------------

namespace
{

}	// namespace


__fastcall TfrmCH341I2CAs5600::TfrmCH341I2CAs5600(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);

	for (unsigned int i=0; i<128; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbI2CAddress->Items->Add(text);
	}
	cbI2CAddress->ItemIndex = AS5600::AS5600_ADDRESS;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CAs5600::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CAs5600::Read(void)
{
	BtnController btnCtrl(btnRead);

	edRawAngle->Text = "";

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = static_cast<uint8_t>(cbI2CAddress->ItemIndex);

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending device address!";
		return;
	}

	uint16_t rawAngle = 0, angle = 0;
	bool magnetDetected = false, magnetTooStrong = false, magnetTooWeak = false;
	AS5600::read(address, rawAngle, angle, magnetDetected, magnetTooStrong, magnetTooWeak);

	lblStatus->Caption = "Read OK";
	AnsiString str;
	str.sprintf("0x%04X => %.1f deg", static_cast<unsigned int>(rawAngle), AS5600::RAW_ANGLE_TO_DEGREES * rawAngle);
	edRawAngle->Text = str;

	AnsiString statusText;
	statusText.cat_sprintf("Magnet %sdetected%s%s", magnetDetected?"":"NOT ",
		magnetTooStrong?", magnet too strong":"", magnetTooWeak?", magnet too weak":"");
	edSensorStatus->Text = statusText;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CAs5600::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CAs5600::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CAs5600::chbAutoReadKeyPress(TObject *Sender,
      char &Key)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

