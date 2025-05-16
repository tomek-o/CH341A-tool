//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341DS3231.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "common/StaticCheck.h"
#include "common/ArraySize.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341DS3231 *frmCH341DS3231;
//---------------------------------------------------------------------------

#ifdef __BORLANDC__
#pragma warn -8071
#endif


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
	REG_MONTH_CENTURY = 5,			// 1...12
	REG_YEAR = 6,			// 00...99

	REG_CONTROL = 0x0E,		// EOSC BBSQW CONV RS2 RS1 INTCN A2IE A1IE 
		REG_CONTROL_BIT_EOSC = (1<<7),	// if set to 1: disabling oscillator when switching to Vbat
		REG_CONTROL_BIT_BBSQW = (1<<6),	// Battery-Backed Square-Wave Enable
		REG_CONTROL_BIT_CONV = (1<<5),	// convert temperature and execute TCXO compensation cycle; not affecting internal 64s measurement cycle
		REG_CONTROL_BIT_RS2 = (1<<4),	// rate select for square wave: 00 -> 1 Hz, 01 -> 1024 Hz, 10 -> 4096 Hz, 11 -> 8192 Hz
		REG_CONTROL_BIT_RS1 = (1<<3),
		REG_CONTROL_BIT_INTCN = (1<<2),	// interrupt control; 0 -> square wave output 1 -> alarm output
		REG_CONTROL_BIT_A2IE = (1<<1),	// alarm 2 enable
		REG_CONTROL_BIT_A1IE = (1<<0),	// alarm 1 enable
	REG_CONTROL_STATUS = 0x0F,	// OSF 0 0 0 EN32kHz BSY A2F A1F
		REG_CONTROL_BIT_OSF = (1<<7),	// oscillator stop flag
		REG_CONTROL_BIT_32K = (1<<3),	// enable 32 kHz output
		REG_CONTROL_BIT_BSY = (1<<2),	// BUSY: executing TCXO cycle
		REG_CONTROL_BIT_A2F = (1<<1),	// alarm 2 flag
		REG_CONTROL_BIT_A1F = (1<<0),	// alarm 1 flag

    REG_AGING_OFFSET = 0x10,	// SIGN DATA DATA DATA DATA DATA DATA DATA

	REG_TEMPERATURE_MSB = 0x11,	// SIGN DATA DATA DATA DATA DATA DATA DATA
	REG_TEMPERATURE_LSB = 0x12,	// DATA DATA 0 0 0 0 0 0 
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
	{ 0x00, "square wave 1 Hz"},
	{ REG_CONTROL_BIT_RS1, "square wave 1024 Hz"},
	{ REG_CONTROL_BIT_RS2, "square wave 4096 Hz"},
	{ REG_CONTROL_BIT_RS2 | REG_CONTROL_BIT_RS1, "square wave 8192 Hz"},
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


__fastcall TfrmCH341DS3231::TfrmCH341DS3231(TComponent* Owner)
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


void __fastcall TfrmCH341DS3231::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341DS3231::Read(void)
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
	text.sprintf("%04d.%02d.%02d", 2000 + bcd2bin(buffer[REG_YEAR]), bcd2bin(buffer[REG_MONTH_CENTURY]), bcd2bin(buffer[REG_DAY]));
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

	{
		int16_t temperature;
		int status = ch341a.I2CWriteCommandReadBuffer(ADDRESS, REG_TEMPERATURE_MSB, reinterpret_cast<unsigned char*>(&temperature), sizeof(temperature));
		if (status != 0)
		{
			lblStatus->Caption = "I2C read failed!";
			return;
		}
		// swap byte order
		uint8_t *ptr = reinterpret_cast<uint8_t*>(&temperature);
		uint8_t tmp = ptr[0];
		ptr[0] = ptr[1];
		ptr[1] = tmp;
		text.cat_printf(", temp %.2f C", static_cast<double>(temperature) / 256.0); 
	}

	lblDateTime->Caption = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341DS3231::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341DS3231::btnSetCurrentTimeClick(TObject *Sender)
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
	buffer[REG_HOURS] = bin2bcd(SystemTime.wHour);				// 00...23
	buffer[REG_WEEKDAY] = bin2bcd(SystemTime.wDayOfWeek + 1);  // Day of week, any day can be first, counts 1...7
	buffer[REG_DAY] = bin2bcd(SystemTime.wDay); // Day of month, 1...31
	buffer[REG_MONTH_CENTURY] = bin2bcd(SystemTime.wMonth);  // month 1...12
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

void __fastcall TfrmCH341DS3231::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS3231::chbAutoReadMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS3231::btnSetControlClick(TObject *Sender)
{
	BtnController btnCtrl(btnSetControl);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	int status = ch341a.I2CWriteCommandWriteByte(ADDRESS, REG_CONTROL,
		(chbSqwOutput->Checked ? 0x00 : REG_CONTROL_BIT_INTCN) |
		(chbBbsqw->Checked ? REG_CONTROL_BIT_BBSQW : 0x00) |
		sqwOutModes[cbControl->ItemIndex].value |
		(chbTemperatureConv->Checked ? REG_CONTROL_BIT_CONV : 0x00)
		);
	if (status != 0)
	{
		lblStatus->Caption = "I2C write failed!";
		return;
	}
	status = ch341a.I2CWriteCommandWriteByte(ADDRESS, REG_CONTROL_STATUS, chbEnable32k->Checked ? REG_CONTROL_BIT_32K : 0x00);
	if (status != 0)
	{
		lblStatus->Caption = "I2C write failed!";
		return;
	}
	lblStatus->Caption = "";
}
//---------------------------------------------------------------------------

