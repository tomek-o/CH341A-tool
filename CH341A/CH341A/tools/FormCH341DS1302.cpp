//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341DS1302.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341DS1302 *frmCH341DS1302;
//---------------------------------------------------------------------------

#ifdef __BORLANDC__
#pragma warn -8071
#endif

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))


namespace
{

// DS1302 RTC
// Open Source / Public Domain
//     By arduino.cc user "Krodal"
//     March 2013
//
// Documentation: datasheet
// 
// The DS1302 uses a 3-wire interface: 
//    - bidirectional data.
//    - clock
//    - chip select
// It is not I2C, not OneWire, and not SPI.
// So the standard libraries can not be used.
// Even the shiftOut() function is not used, since it
// could be too fast (it might be slow enough, 
// but that's not certain).
//
// I wrote my own interface code according to the datasheet.
// Any three pins of the Arduino can be used.
//   See the first defines below this comment, 
//   to set your own pins.
//
// The "Chip Enable" pin was called "/Reset" before.
//
// The chip has internal pull-down registers.
// This keeps the chip disabled, even if the pins of 
// the Arduino are floating.
//
//
// Range
// -----
//      seconds : 00-59
//      minutes : 00-59
//      hour    : 1-12 or 0-23
//      date    : 1-31
//      month   : 1-12
//      day     : 1-7
//      year    : 00-99
//
//
// Burst mode
// ----------
// In burst mode, all the clock data is read at once.
// This is to prevent a rollover of a digit during reading.
// The read data is from an internal buffer.
//
// The burst registers are commands, rather than addresses.
// Clock Data Read in Burst Mode
//    Start by writing 0xBF (as the address), 
//    after that: read clock data
// Clock Data Write in Burst Mode
//    Start by writing 0xBE (as the address), 
//    after that: write clock data
// Ram Data Read in Burst Mode
//    Start by writing 0xFF (as the address), 
//    after that: read ram data
// Ram Data Write in Burst Mode
//    Start by writing 0xFE (as the address), 
//    after that: write ram data
//
//
// Ram
// ---
// The DS1302 has 31 of ram, which can be used to store data.
// The contents will be lost if the Arduino is off, 
// and the backup battery gets empty.
// It is better to store data in the EEPROM of the Arduino.
// The burst read or burst write for ram is not implemented 
// in this code.
//
//
// Trickle charge
// --------------
// The DS1302 has a build-in trickle charger.
// That can be used for example with a lithium battery 
// or a supercap.
// Using the trickle charger has not been implemented 
// in this code.
//

enum
{
	CE_PIN_ID = 0,
	SCLK_PIN_ID = 18,		// SCLK = SCL, output only
	DATA_OUT_PIN_ID = 19,	// IO = SDA, semi-bidirectional with open-drain output and pull-up
	DATA_IN_PIN_ID = 23		// IO = SDA, separate bit required for reading input
};

bool sclkState = false;
bool ioDirOut = false;
bool ioState = false;
bool ceState = false;

void update(void)
{
	uint32_t direction = (1u << SCLK_PIN_ID) | (1u << CE_PIN_ID);
	if (ioDirOut)
		direction |= (1u << DATA_OUT_PIN_ID);
	uint32_t value = 0;
	if (sclkState)
		value |= (1u << SCLK_PIN_ID);
	if (ioState)
		value |= (1u << DATA_OUT_PIN_ID);
	if (ceState)
		value |= (1u << CE_PIN_ID);

	ch341a.SetGpioOutputs(direction, value);
}


// Macros to convert the bcd values of the registers to normal
// integer variables.
// The code uses separate variables for the high byte and the low byte
// of the bcd, so these macros handle both bytes separately.
#define bcd2bin(h,l)    (((h)*10) + (l))
#define bin2bcd_h(x)   ((x)/10)
#define bin2bcd_l(x)    ((x)%10)


// Register names.
// Since the highest bit is always '1', 
// the registers start at 0x80
// If the register is read, the lowest bit should be '1'.
#define DS1302_SECONDS           0x80
#define DS1302_MINUTES           0x82
#define DS1302_HOURS             0x84
#define DS1302_DATE              0x86
#define DS1302_MONTH             0x88
#define DS1302_DAY               0x8A
#define DS1302_YEAR              0x8C
#define DS1302_ENABLE            0x8E
#define DS1302_TRICKLE           0x90
#define DS1302_CLOCK_BURST       0xBE
#define DS1302_CLOCK_BURST_WRITE 0xBE
#define DS1302_CLOCK_BURST_READ  0xBF
#define DS1302_RAMSTART          0xC0
#define DS1302_RAMEND            0xFC
#define DS1302_RAM_BURST         0xFE
#define DS1302_RAM_BURST_WRITE   0xFE
#define DS1302_RAM_BURST_READ    0xFF

/*
X X X X X X 0 0 	Disabled
X X X X 0 0 X X 	Disabled
X X X X 1 1 X X 	Disabled
1 0 1 0 0 1 0 1     1 Diode, 2k
1 0 1 0 0 1 1 0     1 Diode, 4k
1 0 1 0 0 1 1 1		1 Diode, 8k
1 0 1 0 1 0 0 1 	2 Diodes, 2k
1 0 1 0 1 0 1 0 	2 Diodes, 4k
1 0 1 0 1 0 1 1 	2 Diodes, 8k
*/
struct TrickleChargingMode {
	uint8_t value;
	const char* description;
} trickleChargingModes[] = {
	{ 0x00, "charging disabled" },
	{ 0xA5, "through diode + 2 kOhm"},
	{ 0xA6, "through diode + 4 kOhm"},
	{ 0xA7, "through diode + 8 kOhm"},
	{ 0xA9, "through 2 diodes + 2 kOhm"},
	{ 0xAA, "through 2 diodes + 4 kOhm"},
	{ 0xAB, "through 2 diodes + 8 kOhm"},
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


// Defines for the bits, to be able to change 
// between bit number and binary definition.
// By using the bit number, using the DS1302 
// is like programming an AVR microcontroller.
// But instead of using "(1<<X)", or "_BV(X)", 
// the Arduino "bit(X)" is used.
#define DS1302_D0 0
#define DS1302_D1 1
#define DS1302_D2 2
#define DS1302_D3 3
#define DS1302_D4 4
#define DS1302_D5 5
#define DS1302_D6 6
#define DS1302_D7 7


// Bit for reading (bit in address)
#define DS1302_READBIT DS1302_D0 // READBIT=1: read instruction

// Bit for clock (0) or ram (1) area, 
// called R/C-bit (bit in address)
#define DS1302_RC DS1302_D6

// Seconds Register
#define DS1302_CH DS1302_D7   // 1 = Clock Halt, 0 = start

// Hour Register
#define DS1302_AM_PM DS1302_D5 // 0 = AM, 1 = PM
#define DS1302_12_24 DS1302_D7 // 0 = 24 hour, 1 = 12 hour

// Enable Register
#define DS1302_WP DS1302_D7   // 1 = Write Protect, 0 = enabled

// Trickle Register
#define DS1302_ROUT0 DS1302_D0
#define DS1302_ROUT1 DS1302_D1
#define DS1302_DS0   DS1302_D2
#define DS1302_DS1   DS1302_D2
#define DS1302_TCS0  DS1302_D4
#define DS1302_TCS1  DS1302_D5
#define DS1302_TCS2  DS1302_D6
#define DS1302_TCS3  DS1302_D7


// Structure for the first 8 registers.
// These 8 bytes can be read at once with 
// the 'clock burst' command.
// Note that this structure contains an anonymous union.
// It might cause a problem on other compilers.
struct ds1302_struct
{
  uint8_t Seconds:4;      // low decimal digit 0-9
  uint8_t Seconds10:3;    // high decimal digit 0-5
  uint8_t CH:1;           // CH = Clock Halt
  uint8_t Minutes:4;
  uint8_t Minutes10:3;
  uint8_t reserved1:1;
  union
  {
    struct
    {
      uint8_t Hour:4;
      uint8_t Hour10:2;
      uint8_t reserved2:1;
      uint8_t hour_12_24:1; // 0 for 24 hour format
    } h24;
    struct
    {
      uint8_t Hour:4;
      uint8_t Hour10:1;
      uint8_t AM_PM:1;      // 0 for AM, 1 for PM
      uint8_t reserved2:1;
      uint8_t hour_12_24:1; // 1 for 12 hour format
    } h12;
  };
  uint8_t Date:4;           // Day of month, 1 = first day
  uint8_t Date10:2;
  uint8_t reserved3:2;
  uint8_t Month:4;          // Month, 1 = January
  uint8_t Month10:1;
  uint8_t reserved4:3;
  uint8_t Day:3;            // Day of week, 1 = first day (any day)
  uint8_t reserved5:5;
  uint8_t Year:4;           // Year, 0 = year 2000
  uint8_t Year10:4;
  uint8_t reserved6:7;
  uint8_t WP:1;             // WP = Write Protect
};


// --------------------------------------------------------
// _DS1302_start
//
// A helper function to setup the start condition.
//
// An 'init' function is not used.
// But now the pinMode is set every time.
// That's not a big deal, and it's valid.
// At startup, the pins of the Arduino are high impedance.
// Since the DS1302 has pull-down resistors, 
// the signals are low (inactive) until the DS1302 is used.
void _DS1302_start( void)
{
	ceState = false;
	update();	// default, not enabled

	sclkState = false;
	update();	// default, clock low

	ioDirOut = true;
	update();

	ceState = true;
	update();
	Sleep(1);
	// start the session
}


// --------------------------------------------------------
// _DS1302_stop
//
// A helper function to finish the communication.
//
void _DS1302_stop(void)
{
	// Set CE low
	ceState = false;
	update();
	Sleep(1);
}

bool ReadInput(void)
{
	uint32_t dataIn = 0;

	int status = ch341a.GetGpioInputs(dataIn);
	if (status != 0)
	{
		//LOG("Reading inputs failed!\n");
		return false;
	}
	if (dataIn & (1u << DATA_IN_PIN_ID))
		return true;
	return false;
}


// --------------------------------------------------------
// _DS1302_toggleread
//
// A helper function for reading a byte with bit toggle
//
// This function assumes that the SCLK is still high.
//
uint8_t _DS1302_toggleread( void)
{
  uint8_t i, data;

  data = 0;
  for( i = 0; i <= 7; i++)
  {
    // Issue a clock pulse for the next databit.
    // If the 'togglewrite' function was used before 
	// this function, the SCLK is already high.
	sclkState = true;
	update();
	Sleep(1);

	// Clock down, data is ready after some time.
	sclkState = false;
	update();
	Sleep(1);	// tCL=1000ns, tCDD=800ns

	data |= static_cast<uint8_t>(ReadInput() ? (1u<<i) : 0);
  }
  return( data);
}


// --------------------------------------------------------
// _DS1302_togglewrite
//
// A helper function for writing a byte with bit toggle
//
// The 'release' parameter is for a read after this write.
// It will release the I/O-line and will keep the SCLK high.
//
void _DS1302_togglewrite( uint8_t data, uint8_t release)
{
  int i;

  for( i = 0; i <= 7; i++)
  { 
    // set a bit of the data on the I/O-line
	ioState = data & (1<<i);
	update();
	Sleep(1);	// tDC = 200ns

	// clock up, data is read by DS1302
	sclkState = true;
	update();
	Sleep(1);	// tCH = 1000ns, tCDH = 800ns

    if( release && i == 7)
    {
      // If this write is followed by a read, 
      // the I/O-line should be released after 
      // the last bit, before the clock line is made low.
      // This is according the datasheet.
      // I have seen other programs that don't release 
      // the I/O-line at this moment,
      // and that could cause a shortcut spike 
      // on the I/O-line.
	  ioDirOut = false;
	  update();

	  // For Arduino 1.0.3, removing the pull-up is no longer needed.
	  // Setting the pin as 'INPUT' will already remove the pull-up.
	  // digitalWrite (DS1302_IO, LOW); // remove any pull-up
	}
	else
	{
		sclkState = false;
		update();
		Sleep(1);	// tCL=1000ns, tCDD=800ns
    }
  }
}

// --------------------------------------------------------
// DS1302_clock_burst_read
//
// This function reads 8 bytes clock data in burst mode
// from the DS1302.
//
// This function may be called as the first function, 
// also the pinMode is set.
//
void DS1302_clock_burst_read( uint8_t *p)
{
  int i;

  _DS1302_start();

  // Instead of the address, 
  // the CLOCK_BURST_READ command is issued
  // the I/O-line is released for the data
  _DS1302_togglewrite( DS1302_CLOCK_BURST_READ, true);  

  for( i=0; i<8; i++)
  {
    *p++ = _DS1302_toggleread();
  }
  _DS1302_stop();
}


// --------------------------------------------------------
// DS1302_clock_burst_write
//
// This function writes 8 bytes clock data in burst mode
// to the DS1302.
//
// This function may be called as the first function, 
// also the pinMode is set.
//
void DS1302_clock_burst_write( uint8_t *p)
{
  int i;

  _DS1302_start();

  // Instead of the address, 
  // the CLOCK_BURST_WRITE command is issued.
  // the I/O-line is not released
  _DS1302_togglewrite( DS1302_CLOCK_BURST_WRITE, false);  

  for( i=0; i<8; i++)
  {
    // the I/O-line is not released
    _DS1302_togglewrite( *p++, false);  
  }
  _DS1302_stop();
}


// --------------------------------------------------------
// DS1302_read
//
// This function reads a byte from the DS1302 
// (clock or ram).
//
// The address could be like "0x80" or "0x81", 
// the lowest bit is set anyway.
//
// This function may be called as the first function, 
// also the pinMode is set.
//
uint8_t DS1302_read(int address)
{
  uint8_t data;

  // set lowest bit (read bit) in address
  address |= (1<<DS1302_READBIT);

  _DS1302_start();
  // the I/O-line is released for the data
  _DS1302_togglewrite( address, true);  
  data = _DS1302_toggleread();
  _DS1302_stop();

  return (data);
}


// --------------------------------------------------------
// DS1302_write
//
// This function writes a byte to the DS1302 (clock or ram).
//
// The address could be like "0x80" or "0x81", 
// the lowest bit is cleared anyway.
//
// This function may be called as the first function, 
// also the pinMode is set.
//
void DS1302_write( int address, uint8_t data)
{
  // clear lowest bit (read bit) in address
  address &= ~(1 << DS1302_READBIT);   

  _DS1302_start();
  // don't release the I/O-line
  _DS1302_togglewrite( address, false); 
  // don't release the I/O-line
  _DS1302_togglewrite( data, false); 
  _DS1302_stop();  
}


}


__fastcall TfrmCH341DS1302::TfrmCH341DS1302(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
	cbTrickleChargeMode->Clear();
	for (unsigned int i=0; i<ARRAY_SIZE(trickleChargingModes); i++)
	{
    	cbTrickleChargeMode->Items->Add(trickleChargingModes[i].description);
	}
	cbTrickleChargeMode->ItemIndex = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS1302::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	sclkState = false;
	ioDirOut = false;
	ioState = false;
	ceState = false;
	update();

	// Start by clearing the Write Protect bit
	// Otherwise the clock data cannot be written
	// The whole register is written,
	// but the WP-bit is the only bit in that register.
	DS1302_write (DS1302_ENABLE, 0);

}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS1302::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341DS1302::Read(void)
{
	BtnController btnCtrl(btnRead);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	ds1302_struct rtc;

	// Read all clock data at once (burst mode).
	DS1302_clock_burst_read( (uint8_t *) &rtc);

	AnsiString text;
	text.sprintf("%04d.%02d.%02d  %02d:%02d:%02d, %s",
		2000 + bcd2bin(rtc.Year10, rtc.Year), bcd2bin(rtc.Month10, rtc.Month), bcd2bin(rtc.Date10, rtc.Date),
		bcd2bin(rtc.h24.Hour10, rtc.h24.Hour), bcd2bin(rtc.Minutes10, rtc.Minutes), bcd2bin(rtc.Seconds10, rtc.Seconds),
		GetWeekdayName(rtc.Day)
	);

	lblDateTime->Caption = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341DS1302::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS1302::btnSetTrickleChargeClick(TObject *Sender)
{
	BtnController btnCtrl(btnSetTrickleCharge);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	DS1302_write(DS1302_TRICKLE, trickleChargingModes[cbTrickleChargeMode->ItemIndex].value);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS1302::btnSetCurrentTimeClick(TObject *Sender)
{
	BtnController btnCtrl(btnSetCurrentTime);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	ds1302_struct rtc;

	// Fill these variables with the date and time.
	int seconds, minutes, hours, dayofweek, dayofmonth, month, year;

	SYSTEMTIME SystemTime;
	::GetLocalTime(&SystemTime);

	seconds    = SystemTime.wSecond;
	minutes    = SystemTime.wMinute;
	hours      = SystemTime.wHour + 1;
	dayofweek  = SystemTime.wDayOfWeek + 1;  // Day of week, any day can be first, counts 1...7
	dayofmonth = SystemTime.wDay; // Day of month, 1...31
	month      = SystemTime.wMonth;  // month 1...12
	year       = SystemTime.wYear;

	// Set a time and date
	// This also clears the CH (Clock Halt) bit,
	// to start the clock.

	// Fill the structure with zeros to make
	// any unused bits zero
	memset ((char *) &rtc, 0, sizeof(rtc));

	rtc.Seconds    = bin2bcd_l(seconds);
	rtc.Seconds10  = bin2bcd_h(seconds);
	rtc.CH         = 0;      // 1 for Clock Halt, 0 to run;
	rtc.Minutes    = bin2bcd_l(minutes);
	rtc.Minutes10  = bin2bcd_h(minutes);
	// To use the 12 hour format,
	// use it like these four lines:
	//    rtc.h12.Hour   = bin2bcd_l( hours);
	//    rtc.h12.Hour10 = bin2bcd_h( hours);
	//    rtc.h12.AM_PM  = 0;     // AM = 0
	//    rtc.h12.hour_12_24 = 1; // 1 for 24 hour format
	rtc.h24.Hour   = bin2bcd_l(hours);
	rtc.h24.Hour10 = bin2bcd_h(hours);
	rtc.h24.hour_12_24 = 0; // 0 for 24 hour format
	rtc.Date       = bin2bcd_l(dayofmonth);
	rtc.Date10     = bin2bcd_h(dayofmonth);
	rtc.Month      = bin2bcd_l(month);
	rtc.Month10    = bin2bcd_h(month);
	rtc.Day        = dayofweek;
	rtc.Year       = bin2bcd_l(year - 2000);
	rtc.Year10     = bin2bcd_h(year - 2000);
	rtc.WP = 0;

	// Write all clock data at once (burst mode).
	DS1302_clock_burst_write( (uint8_t *) &rtc);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS1302::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341DS1302::chbAutoReadMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

