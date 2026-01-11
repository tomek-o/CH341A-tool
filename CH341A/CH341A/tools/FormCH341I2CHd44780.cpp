//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CHd44780.h"
#include "CH341A.h"
#include "HD44780.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CHd44780 *frmCH341I2CHd44780;
//---------------------------------------------------------------------------

using namespace HD44780;

namespace
{

/**
Connections:
	D0 = RS
	D1 = RW
	D2 = ENABLE
	D3 = LED control
	D4 = DATA4
	D5 = DATA5
	D6 = DATA6
	D7 = DATA7
*/

class Lcd
{
private:
	enum {
		BIT_RS = 0,
		BIT_RW = 1,
		BIT_ENABLE = 2,
		BIT_DATA4 = 4,
		BIT_DATA5 = 5,
		BIT_DATA6 = 6,
		BIT_DATA7 = 7,

		BIT_LED = 3
	};
	uint8_t address;
	uint8_t data;
	int updatePins(void) {
		uint8_t buf[2];
		buf[0] = static_cast<uint8_t>(address << 1);
		buf[1] = data;
		return ch341a.I2CWriteRead(buf, sizeof(buf), NULL, 0);
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
		Sleep(1);

		data &= ~(1<<BIT_ENABLE);
		updatePins();
		Sleep(1);																// If delay less than this value, the data is not correctly displayed
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
		address(0),
		data(1<<BIT_LED)	// RW = low for writing
	{
	}
	void setAddress(uint8_t address)
	{
		this->address = address;
	}
	
	int init(AnsiString line1, AnsiString line2)
	{
		if (!ch341a.IsOpened())
		{
			return -1;
		}
		int status;
		data &= (1<<BIT_LED);	// reset all bits but LED
		status = updatePins();
		if (status != 0)
			return status;

		Sleep(10);

		for (unsigned int i=0; i<3; i++)
		{
			writeNibble(0x03);
			Sleep(5);
		}

		writeNibble(0x02);	// enable 4 bit mode

		write(CMD_FUNCTIONSET | FUNCTION_4BITMODE | FUNCTION_2LINE | FUNCTION_5x8DOTS, false);
		write(CMD_DISPLAYCONTROL | CONTROL_DISPLAYON /* | CONTROL_CURSORON | CONTROL_BLINKON */, false);
		write(CMD_CLEARDISPLAY, false);
		write(CMD_RETURNHOME, false);

		for (int i=1; i<=line1.Length(); i++)
		{
			write(static_cast<uint8_t>(line1[i]), true);
		}

		write(0xc0, false);			// go to next line

		for (int i=1; i<=line2.Length(); i++)
		{
			write(static_cast<uint8_t>(line2[i]), true);
		}

		return status;
	}

	void setBacklight(bool state)
	{
		if (state)
		{
			data |= (1<<BIT_LED);
		}
		else
		{
        	data &= ~(1<<BIT_LED);
		}
		updatePins();
	}
} lcd;

}	// namespace


__fastcall TfrmCH341I2CHd44780::TfrmCH341I2CHd44780(TComponent* Owner)
	: TForm(Owner)
{
	for (unsigned int i=0; i<=128; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbI2CAddress->Items->Add(text);
	}
	cbI2CAddress->ItemIndex = 0x27;

	TabManager::Instance().Register(this, 1u << ToolGroupDisplays);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CHd44780::btnWriteClick(TObject *Sender)
{
	Write();
}

void TfrmCH341I2CHd44780::Write(void)
{
	BtnController btnCtrl(btnWrite);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = static_cast<uint8_t>(cbI2CAddress->ItemIndex);

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	lblStatus->Caption = "";

    lcd.setAddress(address);
	lcd.init(edLine1->Text, edLine2->Text);
}
//---------------------------------------------------------------------------

void TfrmCH341I2CHd44780::SetBacklight(bool state)
{
	BtnController btnCtrlOn(btnBacklightOn);
	BtnController btnCtrlOff(btnBacklightOff);
	
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = static_cast<uint8_t>(cbI2CAddress->ItemIndex);

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	lblStatus->Caption = "";

	lcd.setAddress(address);
	lcd.setBacklight(state);
}

void __fastcall TfrmCH341I2CHd44780::btnBacklightOnClick(TObject *Sender)
{
	SetBacklight(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CHd44780::btnBacklightOffClick(TObject *Sender)
{
	SetBacklight(false);
}
//---------------------------------------------------------------------------

