//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341DS1307.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "common/StaticCheck.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341DS1307 *frmCH341DS1307;
//---------------------------------------------------------------------------

#ifdef __BORLANDC__
#pragma warn -8071
#endif

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))


namespace
{

enum
{
	ADDRESS = 0x68
};

/** \note Numbers are coded as BCD. */
enum
{
	REG_SECONDS = 0,
		REG_SECONDS_BIT_CLOCK_HALT = (1<<7),
		REG_SECONDS_VALUE_MASK = ~(REG_SECONDS_BIT_CLOCK_HALT),
	REG_MINUTES = 1,
	REG_HOURS = 2,			// 1...12 (if bit 6 is set) or 0...23 (if bit 6 = 0)
		REG_HOURS_BIT_AM_PM_MODE = (1<<6),
		REG_HOURS_BIT_AM_PM = (1<<5),
	REG_WEEKDAY = 3,		// 1...7
	REG_DAY = 4,			// 1...31
	REG_MONTH = 5,			// 1...12
	REG_YEAR = 6,			// 00...99

	REG_CONTROL = 7,
		REG_CONTROL_BIT_OUT = (1<<7),	// output state if square wave generator function is not enabled
		REG_CONTROL_BIT_SQWE = (1<<4),	// enable square wave output (pull-up required, open-drain)
		REG_CONTROL_BIT_RS1 = (1<<1),	// rate select for square wave: 00 -> 1 Hz, 01 -> 4096 Hz, 10 -> 8192 Hz, 11 -> 32768 Hz
		REG_CONTROL_BIT_RS0 = (1<<0),
	
	REG_RAM_FIRST = 0x08,	// 56 bytes
	REG_RAM_LAST = 0x3F
};

const char* GetWeekdayName(int val)
{
	switch (val)
	{
	case 1:
		return "Sunday";
	case 2:
		return "Monday";
	case 3:
		return "Tuesday";
	case 4:
		return "Wednesday";
	case 5:
		return "Thursday";
	case 6:
		return "Friday";
	case 7:
		return "Saturday";
	default:
		return "???";
	}
}

struct SqwOutMode {
	uint8_t value;
	const char* description;
} sqwOutModes[] = {
	{ REG_CONTROL_BIT_OUT, "level = 1"},
	{ 0x00, "level = 0" },
	{ REG_CONTROL_BIT_SQWE, "square wave 1 Hz"},
	{ REG_CONTROL_BIT_SQWE | REG_CONTROL_BIT_RS0, "square wave 4096 Hz"},
	{ REG_CONTROL_BIT_SQWE | REG_CONTROL_BIT_RS1, "square wave 8192 Hz"},
	{ REG_CONTROL_BIT_SQWE | REG_CONTROL_BIT_RS0 | REG_CONTROL_BIT_RS1, "square wave 32768 Hz"},
};

int bcd2bin(uint8_t val)
{
	return (((val & 0xF0) >> 4) * 10) + (val & 0x0F); 
}

uint8_t bin2bcd(int val)
{
	return ((val/10)<<4) + (val%10);
}

}	// namespace


__fastcall TfrmCH341DS1307::TfrmCH341DS1307(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
	for (unsigned int i=0; i<ARRAY_SIZE(sqwOutModes); i++)
	{
		cbControl->Items->Add(sqwOutModes[i].description);
	}
	cbControl->ItemIndex = 0;
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341DS1307::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341DS1307::Read(void)
{
	BtnController btnCtrl(btnRead);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t buffer[REG_YEAR - REG_SECONDS + 1];
	int status = ch341a.I2CWriteCommandReadBuffer(ADDRESS, REG_SECONDS, buffer, sizeof(buffer));
	if (status != 0)
	{
		lblStatus->Caption = "I2C read failed!";
		return;
	}
	else
	{
    	lblStatus->Caption = "";
	}	
	STATIC_CHECK(REG_SECONDS == 0, UnexpectedRegisterStart);

	AnsiString text;
	text.sprintf("%04d.%02d.%02d", 2000 + bcd2bin(buffer[REG_YEAR]), bcd2bin(buffer[REG_MONTH]), bcd2bin(buffer[REG_DAY]));
	if (buffer[REG_HOURS] & REG_HOURS_BIT_AM_PM_MODE)
	{
		bool pm = buffer[REG_HOURS] & REG_HOURS_BIT_AM_PM;
		uint8_t numericVal = buffer[REG_HOURS] & 0x1F;
		text.cat_printf("  %d:%02d:%02d %s",
			bcd2bin(numericVal), bcd2bin(buffer[REG_MINUTES]),
			bcd2bin(buffer[REG_SECONDS] & REG_SECONDS_VALUE_MASK),
			pm ? "PM" : "AM");
	}
	else
	{
		uint8_t numericVal = buffer[REG_HOURS] & 0x3F;
		text.cat_printf("  %02d:%02d:%02d",
			bcd2bin(numericVal), bcd2bin(buffer[REG_MINUTES]),
			bcd2bin(buffer[REG_SECONDS] & REG_SECONDS_VALUE_MASK));
	}

	text.cat_printf(", %s", GetWeekdayName(buffer[REG_WEEKDAY]));

	lblDateTime->Caption = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341DS1307::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341DS1307::btnSetCurrentTimeClick(TObject *Sender)
{
	BtnController btnCtrl(btnSetCurrentTime);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t buffer[REG_YEAR - REG_SECONDS + 1];

	SYSTEMTIME SystemTime;
	::GetLocalTime(&SystemTime);

	// Set a time and date
	// This also clears the CH (Clock Halt) bit,
	// to start the clock.

	buffer[REG_SECONDS] = bin2bcd(SystemTime.wSecond);
	buffer[REG_MINUTES] = bin2bcd(SystemTime.wMinute);
	buffer[REG_HOURS] = bin2bcd(SystemTime.wHour + 1);
	buffer[REG_WEEKDAY] = bin2bcd(SystemTime.wDayOfWeek + 1);  // Day of week, any day can be first, counts 1...7
	buffer[REG_DAY] = bin2bcd(SystemTime.wDay); // Day of month, 1...31
	buffer[REG_MONTH] = bin2bcd(SystemTime.wMonth);  // month 1...12
	buffer[REG_YEAR] = bin2bcd(SystemTime.wYear - 2000);

	int status = ch341a.I2CWriteCommandWriteBytes(ADDRESS, REG_SECONDS, buffer, sizeof(buffer));
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

void __fastcall TfrmCH341DS1307::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS1307::chbAutoReadMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS1307::btnSetTrickleChargeClick(TObject *Sender)
{
	BtnController btnCtrl(btnSetTrickleCharge);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	int status = ch341a.I2CWriteCommandWriteByte(ADDRESS, REG_CONTROL, sqwOutModes[cbControl->ItemIndex].value);
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

