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
#include <math.h>
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

	angleRad = (M_PI * 2.0 * rawAngle / 4096.0);
	paintBox->Repaint();
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

void __fastcall TfrmCH341I2CAs5600::paintBoxPaint(TObject *Sender)
{
	TCanvas *c = paintBox->Canvas;

	//c->Font->Orientation = 90;	// rotating fonts with this looks REALLY ugly...

	enum { CIRCLE_OFFSET = 30 };
	c->Ellipse(CIRCLE_OFFSET, CIRCLE_OFFSET, paintBox->Width-CIRCLE_OFFSET, paintBox->Height-CIRCLE_OFFSET);
	const unsigned int centerX = paintBox->Width / 2;
	const unsigned int centerY = paintBox->Height / 2;
	const unsigned int diameter = paintBox->Width - (CIRCLE_OFFSET * 2);
	const float radius = static_cast<float>(diameter) / 2;

	//c->TextOutA(80, 80, "test");

	double deltaX = radius * cos(angleRad);
	double deltaY = radius * sin(angleRad);

	TColor color = c->Pixels[centerX][centerY];

	c->MoveTo(static_cast<int>(centerX - deltaX), static_cast<int>(centerY - deltaY));
	c->LineTo(static_cast<int>(centerX + deltaX), static_cast<int>(centerY + deltaY));

	//LOG("deltaX = %.1f, deltaY = %.1f\n", deltaX, deltaY);

	double dx = 0.66 * radius * cos(angleRad + (M_PI/2));
	double dy = 0.66 * radius * sin(angleRad + (M_PI/2));

	c->Brush->Color = static_cast<TColor>(0x4040FF);
	c->FloodFill(static_cast<int>(centerX - dx), static_cast<int>(centerY - dy), color, fsSurface);
	c->TextOutA(static_cast<int>(centerX - dx - 3), static_cast<int>(centerY - dy - 5), "N");
	c->Brush->Color = static_cast<TColor>(0xFF4040);
	c->FloodFill(static_cast<int>(centerX + dx), static_cast<int>(centerY + dy), color, fsSurface);
	c->TextOutA(static_cast<int>(centerX + dx - 3), static_cast<int>(centerY + dy - 5), "S");
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CAs5600::trbarTestChange(TObject *Sender)
{
	angleRad = (M_PI * trbarTest->Position) / 180.0;
	paintBox->Repaint();
}
//---------------------------------------------------------------------------

