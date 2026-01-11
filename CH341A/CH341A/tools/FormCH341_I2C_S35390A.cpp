//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341_I2C_S35390A.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "ValueDescription.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341_I2C_S35390A *frmCH341_I2C_S35390A;
//---------------------------------------------------------------------------

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

namespace
{

// I2C address: combining base address and register address
const uint8_t ADDR_BASE = 0x30;

const uint8_t REG_STATUS1 = 0x00;
const uint8_t REG_STATUS2 = 0x01;
const uint8_t REG_REAL_TIME1 = 0x02;
const uint8_t REG_REAL_TIME2 = 0x03;
const uint8_t REG_INT1 = 0x04;
const uint8_t REG_INT2 = 0x05;

const int REAL_TIME1_BYTE_YEAR = 0;
const int REAL_TIME1_BYTE_MONTH = 1;
const int REAL_TIME1_BYTE_DAY = 2;
const int REAL_TIME1_BYTE_WDAY = 3;
const int REAL_TIME1_BYTE_HOURS = 4;
const int REAL_TIME1_BYTE_MINS = 5;
const int REAL_TIME1_BYTE_SECS = 6;

#ifdef __BORLANDC__
#pragma warn -8071
#endif

/**	\brief Reverse bit order
	\note Usually I2C would be using MSB first,
	but this device encodes data as BCD, starting from least significant
*/
uint8_t reverse(uint8_t b) {
   b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
   b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
   b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
   return b;
}

uint8_t BinToBcdLsbFirst(int val)
{
	uint8_t ret = ((val/10)<<4) + (val%10);
	return reverse(ret);
}

int BcdLsbFirstToBin(int val)
{
	uint8_t u8 = val;
	u8 = reverse(u8);
	return (((u8) & 0x0f) + ((u8) >> 4) * 10);
}

#ifdef __BORLANDC__
#pragma warn .8071
#endif

// 	Datasheet does not seem to mention arbitrary number <-> day assignment,
//	any would probably work if weekday is not determined by the chip itself.
const char* GetWeekdayName(int val)
{
	switch (val)
	{
	case 0:
		return "Sunday";
	case 1:
		return "Monday";
	case 2:
		return "Tuesday";
	case 3:
		return "Wednesday";
	case 4:
		return "Thursday";
	case 5:
		return "Friday";
	case 6:
		return "Saturday";
	default:
		return "???";
	}
}


enum { INT1_OFFSET = 4 };
const uint8_t INT1_DISABLED = 0x00 << INT1_OFFSET;
const uint8_t INT1_FREQ_OUTPUT = 0x08 << INT1_OFFSET;
const uint8_t INT1_MIN_EDGE_INT = 0x04 << INT1_OFFSET;
const uint8_t INT1_MIN_PER_INT1 = 0x0C << INT1_OFFSET;
const uint8_t INT1_ALARM1 = 0x02 << INT1_OFFSET;
const uint8_t INT1_MIN_PER_INT2 = 0x0E << INT1_OFFSET;
const uint8_t INT1_32768_OUTPUT = 0x01 << INT1_OFFSET;
ValueDescriptionU8 int1Sel[] = {
	{ INT1_DISABLED, "Disabled" },
	{ INT1_FREQ_OUTPUT, "Frequency output" },
	{ INT1_MIN_EDGE_INT, "Per-minute edge interrupt" },
	{ INT1_MIN_PER_INT1, "Minute-periodic interrupt 1 (50%)" },
	{ INT1_ALARM1, "Alarm 1 interrupt" },
	{ INT1_MIN_PER_INT2, "Minute-periodic interrupt 2" },
	{ INT1_32768_OUTPUT, "32.768 kHz output" },
};

enum { INT2_OFFSET = 1 };
const uint8_t INT2_DISABLED = 0x00 << INT2_OFFSET;
const uint8_t INT2_FREQ_OUTPUT = 0x04 << INT2_OFFSET;
const uint8_t INT2_MIN_EDGE_INT = 0x02 << INT2_OFFSET;
const uint8_t INT2_MIN_PER_INT1 = 0x06 << INT2_OFFSET;
const uint8_t INT2_ALARM2 = 0x01 << INT2_OFFSET;
ValueDescriptionU8 int2Sel[] = {
	{ INT2_DISABLED, "Disabled" },
	{ INT2_FREQ_OUTPUT, "Frequency output" },
	{ INT2_MIN_EDGE_INT, "Per-minute edge interrupt" },
	{ INT2_MIN_PER_INT1, "Minute-periodic interrupt 1 (50%)" },
	{ INT2_ALARM2, "Alarm 2 interrupt" },
};

}	// namespace


__fastcall TfrmCH341_I2C_S35390A::TfrmCH341_I2C_S35390A(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupRtc);
	FillComboboxWithValues(int1Sel, cbInt1Sel, INT1_FREQ_OUTPUT);
	FillComboboxWithValues(int2Sel, cbInt2Sel, INT2_FREQ_OUTPUT);
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341_I2C_S35390A::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341_I2C_S35390A::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(ADDR_BASE + REG_STATUS1);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending STATUS1 register address!";
		return;
	}

	uint8_t buffer[7];

	status = ch341a.I2CReadBytes(ADDR_BASE + REG_REAL_TIME1, buffer, sizeof(buffer));
	if (status != 0)
	{
		lblStatus->Caption = "I2C read failed!";
		return;
	}
	else
	{
		lblStatus->Caption = "";
		AnsiString text;
		text.sprintf("%04d.%02d.%02d %02d:%02d:%02d %s",
			BcdLsbFirstToBin(buffer[REAL_TIME1_BYTE_YEAR]) + 2000,
			BcdLsbFirstToBin(buffer[REAL_TIME1_BYTE_MONTH]),
			BcdLsbFirstToBin(buffer[REAL_TIME1_BYTE_DAY]),
			BcdLsbFirstToBin(buffer[REAL_TIME1_BYTE_HOURS] & ~0x02),	// ignore AM/PM bit
			BcdLsbFirstToBin(buffer[REAL_TIME1_BYTE_MINS]),
			BcdLsbFirstToBin(buffer[REAL_TIME1_BYTE_SECS]),
			GetWeekdayName(BcdLsbFirstToBin(buffer[REAL_TIME1_BYTE_WDAY]))
		);
		lblDateTime->Caption = text;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_S35390A::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_S35390A::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_S35390A::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_S35390A::btnSetCurrentTimeClick(TObject *Sender)
{
	BtnController btnCtrl(btnSetCurrentTime);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(ADDR_BASE + REG_STATUS1);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending STATUS1 register address!";
		return;
	}

	{
		// force 24-hour mode
		status = ch341a.I2CWriteByte(ADDR_BASE + REG_STATUS1, 0x40);
		if (status != 0)
		{
			lblStatus->Caption = "Error writing STATUS1 register address!";
			return;
		}
	}	

	uint8_t buffer[7];

	SYSTEMTIME SystemTime;
	::GetLocalTime(&SystemTime);

	buffer[REAL_TIME1_BYTE_SECS] = BinToBcdLsbFirst(SystemTime.wSecond);
	buffer[REAL_TIME1_BYTE_MINS] = BinToBcdLsbFirst(SystemTime.wMinute);
	buffer[REAL_TIME1_BYTE_HOURS] = BinToBcdLsbFirst(SystemTime.wHour);
	buffer[REAL_TIME1_BYTE_WDAY] = BinToBcdLsbFirst(SystemTime.wDayOfWeek);  // Day of week, any day can be first, counts 1...7
	buffer[REAL_TIME1_BYTE_DAY] = BinToBcdLsbFirst(SystemTime.wDay); // Day of month, 1...31
	buffer[REAL_TIME1_BYTE_MONTH] = BinToBcdLsbFirst(SystemTime.wMonth);  // month 1...12
	buffer[REAL_TIME1_BYTE_YEAR] = BinToBcdLsbFirst(SystemTime.wYear - 2000);

	status = ch341a.I2CWriteBytes(ADDR_BASE + REG_REAL_TIME1, buffer, sizeof(buffer));
	if (status != 0)
	{
		lblStatus->Caption = "I2C write failed!";
		return;
	}
	else
	{
		lblStatus->Caption = "";
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_S35390A::cbInt1SelChange(TObject *Sender)
{
	cbInt1Frequency->Visible = (int1Sel[cbInt1Sel->ItemIndex].value == INT1_FREQ_OUTPUT);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_I2C_S35390A::cbInt2SelChange(TObject *Sender)
{
	cbInt2Frequency->Visible = (int2Sel[cbInt2Sel->ItemIndex].value == INT2_FREQ_OUTPUT);
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341_I2C_S35390A::btnSetOutputsConfigClick(TObject *Sender)
{
	BtnController btnCtrl(btnSetOutputsConfig);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(ADDR_BASE + REG_STATUS2);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending STATUS2 register address!";
		return;
	}

	uint8_t val = int1Sel[cbInt1Sel->ItemIndex].value | int2Sel[cbInt2Sel->ItemIndex].value;
	LOG("S35390A: Writing 0x%02X to STATUS2 register\n", val);
	status = ch341a.I2CWriteByte(ADDR_BASE + REG_STATUS2, val);
	if (status != 0)
	{
		lblStatus->Caption = "Error writing STATUS2 register address!";
		return;
	}

	if (int1Sel[cbInt1Sel->ItemIndex].value == INT1_FREQ_OUTPUT)
	{
		val = static_cast<uint8_t>(1 << (7 - cbInt1Frequency->ItemIndex));
		LOG("S35390A: Writing 0x%02X to INT1 register\n", val);
		status = ch341a.I2CWriteByte(ADDR_BASE + REG_INT1, val);
		if (status != 0)
		{
			lblStatus->Caption = "Error writing INT1 register address!";
			return;
		}
	}

	if (int2Sel[cbInt2Sel->ItemIndex].value == INT2_FREQ_OUTPUT)
	{
		val = static_cast<uint8_t>(1 << (7 - cbInt2Frequency->ItemIndex));
		LOG("S35390A: Writing 0x%02X to INT2 register\n", val);
		status = ch341a.I2CWriteByte(ADDR_BASE + REG_INT2, val);
		if (status != 0)
		{
			lblStatus->Caption = "Error writing INT2 register address!";
			return;
		}
	}

	lblStatus->Caption = "";
}
//---------------------------------------------------------------------------

