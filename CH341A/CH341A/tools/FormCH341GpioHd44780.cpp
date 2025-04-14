//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FormCH341GpioHd44780.h"
#include "CH341A.h"
#include "HD44780.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341GpioHd44780 *frmCH341GpioHd44780;
//---------------------------------------------------------------------------

/*
Connections:
CH341A      Display

GND         VSS (pin 1)
+5V         VCC (pin 2) - or 3.3V, depending on display model
RX          Register Select (pin 4); low -> command, high -> data
TX          ENABLE (pin 6); H->L pulse after sending data/cmd, selecting data/cmd reg, selecting write
SPI CLK     DATA4 (pin 11)
SPI CS      DATA5 (pin 12)
SPI MOSI    DATA6 (pin 13)
SPI MISO    DATA7 (pin 14)

Display contrast input might need external potentiometer.
Display R/W (pin 5) should be connected to GND.

*/

using namespace HD44780;

namespace
{

class Lcd
{
private:
	enum {
		BIT_RS = 9,		// RXD
		BIT_ENABLE = 8, // TXD
		BIT_DATA4 = 3,	// SPI CLK
		BIT_DATA5 = 0,	// SPI CS0
		BIT_DATA6 = 5,	// SPI MOSI
		BIT_DATA7 = 7	// SPI MISO
	};
	enum {
		DIRECTION = (1<<BIT_RS)|(1<<BIT_ENABLE)|(1<<BIT_DATA4)|(1<<BIT_DATA5)|(1<<BIT_DATA6)|(1<<BIT_DATA7)
	};
	unsigned int data;
	int updatePins(void) {
		return ch341a.SetGpioOutputs(DIRECTION, data);
	}

	void writeNibble(uint8_t nibble) {
		data &= ~((1<<BIT_DATA4)|(1<<BIT_DATA5)|(1<<BIT_DATA6)|(1<<BIT_DATA7));
		if (nibble & 0x01)
			data |= (1<<BIT_DATA4);
		if (nibble & 0x02)
			data |= (1<<BIT_DATA5);
		if (nibble & 0x04)
			data |= (1<<BIT_DATA6);
		if (nibble & 0x08)
			data |= (1<<BIT_DATA7);
		updatePins();
		Sleep(5);

		data |= (1<<BIT_ENABLE);
		updatePins();
		Sleep(5);

		data &= ~(1<<BIT_ENABLE);
		updatePins();
		Sleep(5);																// If delay less than this value, the data is not correctly displayed
	}
	void write(uint8_t value, bool dataMode) {
		if (dataMode) {
			LOG("HD44780: writing data 0x%02X ('%c')\n", static_cast<unsigned int>(value), value);
			data |= (1 << BIT_RS);
		} else {
			LOG("HD44780: writing command 0x%02X\n", static_cast<unsigned int>(value));
			data &= ~(1 << BIT_RS);
		}
		updatePins();
		Sleep(1);
	#ifdef __BORLANDC__
	#pragma warn -8071
	#endif
		writeNibble((value >> 4) & 0x0F);
		writeNibble(value & 0x0F);
	#ifdef __BORLANDC__
	#pragma warn .8071
	#endif
	}
public:
	Lcd(void):
		data(0)
	{
	}
	int init(void)
	{
		if (!ch341a.IsOpened())
		{
			return -1;
		}
		int status;
		data = 0;
		status = updatePins();
		if (status != 0)
			return status;

		Sleep(10);

		for (unsigned int i=0; i<3; i++)
		{
			writeNibble(0x03);
			Sleep(10);
		}

		writeNibble(0x02);	// enable 4 bit mode

		write(CMD_FUNCTIONSET | FUNCTION_4BITMODE | FUNCTION_2LINE | FUNCTION_5x8DOTS, false);
		write(CMD_DISPLAYCONTROL | CONTROL_DISPLAYON /* | CONTROL_CURSORON | CONTROL_BLINKON */, false);
		write(CMD_CLEARDISPLAY, false);
		write(CMD_RETURNHOME, false);

		for (unsigned int i=0; i<16; i++)
		{
			write(static_cast<uint8_t>('A' + i), true);
		}

		write(0xc0, false);			// go to next line

		for (unsigned int i=0; i<16; i++)
		{
			write(static_cast<uint8_t>('a' + i), true);
		}

		return status;
	}
} lcd;

}	// namespace


__fastcall TfrmCH341GpioHd44780::TfrmCH341GpioHd44780(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
	// delayed wrapping to prevent wrapping at design time to fixed size
	memoInstruction->WordWrap = true;
}
//---------------------------------------------------------------------------

void TfrmCH341GpioHd44780::Write(void)
{
	BtnController btnCtrl(btnWrite);

    lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	lcd.init();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341GpioHd44780::btnWriteClick(TObject *Sender)
{
	Write();	
}
//---------------------------------------------------------------------------

