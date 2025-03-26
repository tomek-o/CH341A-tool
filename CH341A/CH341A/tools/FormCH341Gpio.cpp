//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341Gpio.h"
#include "GpioCfgPanel.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/StaticCheck.h"
#include "common/BtnController.h"
#include "common/TimerController.h"
#include <stdint.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341Gpio *frmCH341Gpio;
//---------------------------------------------------------------------------

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

/*

Pins easily accessible on the CH341A-based EEPROM/FLASH programmer:

D0 	= SPI CS0		= SPI MEM pin 1 (DIP8) - output by default
D3 	= SPI CLK		= SPI MEM pin 6
D5	= SPI MOSI		= SPI MEM pin 5
D7	= SPI MISO		= SPI MEM pin 2

D8	= TXD
D9	= RXD

SDA line is quasi-bidirectional:
- bit 23 = SDA as input (GetInput)
- bit 19 = SDA as output (open drain, SetOutput)

*/

namespace
{

AnsiString GetGpioDescription(unsigned int id)
{
	switch (id)
	{
	case 0:
		return "SPI CS0";
	case 3:
		return "SPI CLK";
	case 5:
		return "SPI MOSI";
	case 7:
		return "SPI MISO";
	case 8:
		return "TXD";
	case 9:
		return "RXD";
	case 10:
		return "INT#";
	case 13:
		return "WAIT#";
	case 14:
		return "DATA#";
	case 15:
		return "ADDRS#";
	case 16:
		return "RESET#, out";
	case 17:
		return "WRITE#, out";
	case 18:
		return "SCL, out";
	case 19:
		return "SDA, out";	// SDA: quasi-bidirectional pin
	case 23:
		return "SDA, in";	// SDA: quasi-bidirectional pin
	default:
		return "";
	}
}

}


__fastcall TfrmCH341Gpio::TfrmCH341Gpio(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);

	STATIC_CHECK(ARRAY_SIZE(panels) == sizeof(uint32_t)*8, UnexpectedNumberOfPanels);

	for (unsigned int i=0; i<ARRAY_SIZE(panels); i++)
	{
		TGpioCfgPanel *pnl = new TGpioCfgPanel(panel, i, GetGpioDescription(i));
		pnl->Parent = panel;
		pnl->Visible = true;
		panels[i] = pnl;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Gpio::btnWriteClick(TObject *Sender)
{
	uint32_t direction = 0;
	uint32_t dataOut = 0;

	STATIC_CHECK(ARRAY_SIZE(panels) == sizeof(uint32_t)*8, UnexpectedNumberOfPanels);

	for (unsigned int i=0; i<ARRAY_SIZE(panels); i++)
	{
		TGpioCfgPanel *pnl = panels[i];
		if (pnl->GetDirection())
			direction |= (1u << i);
		if (pnl->GetDataOut())
			dataOut |= (1u << i);
	}

	int status = ch341a.SetGpioOutputs(direction, dataOut);
	if (status != 0)
	{
		MessageBox(this->Handle, "Error writing GPIO direction/values", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341Gpio::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341Gpio::Read(void)
{
	BtnController btnCtrl(btnRead);
	TimerController tmrCtrl(timerRead);

	uint32_t dataIn = 0;

	int status = ch341a.GetGpioInputs(dataIn);
	if (status != 0)
	{
		MessageBox(this->Handle, "Error reading GPIO states", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	STATIC_CHECK(ARRAY_SIZE(panels) == sizeof(uint32_t)*8, UnexpectedNumberOfPanels);

	for (unsigned int i=0; i<ARRAY_SIZE(panels); i++)
	{
		TGpioCfgPanel *pnl = panels[i];
		pnl->SetDataIn(dataIn & (1u << i));
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341Gpio::timerReadTimer(TObject *Sender)
{
	if (chbReadPeriodically->Checked == false)
		return;
	Read();
}
//---------------------------------------------------------------------------

