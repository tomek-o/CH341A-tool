//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CAds1115.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CAds1115 *frmCH341I2CAds1115;
//---------------------------------------------------------------------------

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

namespace
{

/* \note For 16-bit register MSB is sent first. */

enum ADS1115_I2C_ADDRESS {
	ADS1115_I2C_ADDRESS_GND = 0x48, // address pin tied to GND
	ADS1115_I2C_ADDRESS_VDD = 0x49, // address pin tied to VCC
	ADS1115_I2C_ADDRESS_SDA = 0x4A, // address pin tied to SDA
	ADS1115_I2C_ADDRESS_SCL = 0x4B, // address pin tied to SCL
	ADS1115_I2C_ADDRESS__LIMIT,
	ADS1115_I2C_ADDRESS__START = ADS1115_I2C_ADDRESS_GND
};

const char* GetAds1115AddressDesc(enum ADS1115_I2C_ADDRESS addr)
{
	switch (addr)
	{
	case ADS1115_I2C_ADDRESS_GND:
		return "address pin tied to GND";
	case ADS1115_I2C_ADDRESS_VDD:
		return "address pin tied to VCC";
	case ADS1115_I2C_ADDRESS_SDA:
		return "address pin tied to SDA";
	case ADS1115_I2C_ADDRESS_SCL:
		return "address pin tied to SCL";
	default:
		return "???";
	}
}

/** Four registers accessed through address pointer register.
	Each register has 16 bits.
*/
enum ADS1115_REG {
	ADS1115_REG_CONVERSION = 0x00,		// for ADS1015: 12-bit used
	ADS1115_REG_CONFIG = 0x01,
	ADS1115_REG_LO_THRESHOLD = 0x02,
	ADS1115_REG_HI_THRESHOLD = 0x03
};

enum {
	ADS1115_CONFIG_OS = (1 << 15)		// operational status (when reading, 0 = conversion in progress) or single-shot conversion start (when writing)
};

struct ValueDescription {
	uint16_t value;
	const char* description;
};

/*	These bits configure the input multiplexer.
	These bits serve no function on the ADS1113 and ADS1114. ADS1113 and
	ADS1114 always use inputs AINP = AIN0 and AINN = AIN1.
*/
enum {
	ADS1115_CONFIG_MUX_OFFSET	= 12,
	ADS1115_CONFIG_MUX_P0_N1	= (0x00 << ADS1115_CONFIG_MUX_OFFSET),	// plus: input 0, minus: input 1
	ADS1115_CONFIG_MUX_P0_N3	= (0x01 << ADS1115_CONFIG_MUX_OFFSET),
	ADS1115_CONFIG_MUX_P1_N3	= (0x02 << ADS1115_CONFIG_MUX_OFFSET),
	ADS1115_CONFIG_MUX_P2_N3	= (0x03 << ADS1115_CONFIG_MUX_OFFSET),
	ADS1115_CONFIG_MUX_P0_NGND	= (0x04 << ADS1115_CONFIG_MUX_OFFSET),
	ADS1115_CONFIG_MUX_P1_NGND	= (0x05 << ADS1115_CONFIG_MUX_OFFSET),
	ADS1115_CONFIG_MUX_P2_NGND	= (0x06 << ADS1115_CONFIG_MUX_OFFSET),
	ADS1115_CONFIG_MUX_P3_NGND	= (0x07 << ADS1115_CONFIG_MUX_OFFSET),
};

struct ValueDescription mux[] = {
	{ ADS1115_CONFIG_MUX_P0_N1, "differential: plus = AIN0, minus = AIN1" },
	{ ADS1115_CONFIG_MUX_P0_N3, "differential: plus = AIN0, minus = AIN3" },
	{ ADS1115_CONFIG_MUX_P1_N3, "differential: plus = AIN1, minus = AIN3" },
	{ ADS1115_CONFIG_MUX_P2_N3, "differential: plus = AIN2, minus = AIN3" },
	{ ADS1115_CONFIG_MUX_P0_NGND, "AIN0 referred to GND" },
	{ ADS1115_CONFIG_MUX_P1_NGND, "AIN1 referred to GND" },
	{ ADS1115_CONFIG_MUX_P2_NGND, "AIN2 referred to GND" },
	{ ADS1115_CONFIG_MUX_P3_NGND, "AIN3 referred to GND" },
};

/*  Full signal range: +/-6144mV ... +/- 256mV.
	This parameter expresses the full-scale range of the ADC scaling.
	Do not apply more than VDD + 0.3V to the analog inputs of the device.
*/
enum {
	ADS1115_CONFIG_PGA_OFFSET	= 9,
	ADS1115_CONFIG_PGA_6144MV	= (0x00 << ADS1115_CONFIG_PGA_OFFSET),
	ADS1115_CONFIG_PGA_4096MV	= (0x01 << ADS1115_CONFIG_PGA_OFFSET),
	ADS1115_CONFIG_PGA_2048MV	= (0x02 << ADS1115_CONFIG_PGA_OFFSET),
	ADS1115_CONFIG_PGA_1024MV	= (0x03 << ADS1115_CONFIG_PGA_OFFSET),
	ADS1115_CONFIG_PGA_512MV	= (0x04 << ADS1115_CONFIG_PGA_OFFSET),
	ADS1115_CONFIG_PGA_256MV	= (0x05 << ADS1115_CONFIG_PGA_OFFSET),
	ADS1115_CONFIG_PGA_256MV_2	= (0x06 << ADS1115_CONFIG_PGA_OFFSET),
	ADS1115_CONFIG_PGA_256MV_3	= (0x07 << ADS1115_CONFIG_PGA_OFFSET),
};

struct ValueDescription range[] = {
	{ ADS1115_CONFIG_PGA_6144MV, "+/- 6144 mV" },
	{ ADS1115_CONFIG_PGA_4096MV, "+/- 4096 mV" },
	{ ADS1115_CONFIG_PGA_2048MV, "+/- 2048 mV" },
	{ ADS1115_CONFIG_PGA_1024MV, "+/- 1024 mV" },
	{ ADS1115_CONFIG_PGA_512MV,	 "+/- 512 mV" },
	{ ADS1115_CONFIG_PGA_256MV,  "+/- 256 mV (0x05)" },
	{ ADS1115_CONFIG_PGA_256MV_2,"+/- 256 mV (0x06)" },
	{ ADS1115_CONFIG_PGA_256MV_3,"+/- 256 mV (0x07)" },
};

/*	Continuous/single shot mode
*/
enum {
	ADS1115_CONFIG_MODE_CONTINUOUS = 0,
	ADS1115_CONFIG_MODE_SINGLE = (1 << 8)
};

/*	Sampling data rate
*/
enum {
	ADS1115_CONFIG_DATA_RATE_OFFSET		= 5,
	ADS1115_CONFIG_DATA_RATE_8SPS		= (0x00 << ADS1115_CONFIG_DATA_RATE_OFFSET),
	ADS1115_CONFIG_DATA_RATE_16SPS		= (0x01 << ADS1115_CONFIG_DATA_RATE_OFFSET),
	ADS1115_CONFIG_DATA_RATE_32SPS		= (0x02 << ADS1115_CONFIG_DATA_RATE_OFFSET),
	ADS1115_CONFIG_DATA_RATE_64SPS		= (0x03 << ADS1115_CONFIG_DATA_RATE_OFFSET),
	ADS1115_CONFIG_DATA_RATE_128SPS		= (0x04 << ADS1115_CONFIG_DATA_RATE_OFFSET),	// default
	ADS1115_CONFIG_DATA_RATE_250SPS		= (0x05 << ADS1115_CONFIG_DATA_RATE_OFFSET),
	ADS1115_CONFIG_DATA_RATE_475SPS		= (0x06 << ADS1115_CONFIG_DATA_RATE_OFFSET),
	ADS1115_CONFIG_DATA_RATE_860SPS		= (0x07 << ADS1115_CONFIG_DATA_RATE_OFFSET),
};

struct ValueDescription rate[] = {
	{ ADS1115_CONFIG_DATA_RATE_8SPS,	"8 sps" },
	{ ADS1115_CONFIG_DATA_RATE_16SPS,	"16 sps" },
	{ ADS1115_CONFIG_DATA_RATE_32SPS,	"32 sps" },
	{ ADS1115_CONFIG_DATA_RATE_64SPS,	"64 sps" },
	{ ADS1115_CONFIG_DATA_RATE_128SPS,	"128 sps" },
	{ ADS1115_CONFIG_DATA_RATE_250SPS,	"250 sps" },
	{ ADS1115_CONFIG_DATA_RATE_475SPS,	"475 sps" },
	{ ADS1115_CONFIG_DATA_RATE_860SPS,	"860 sps" },
};

/*	This bit configures the comparator operating mode.
	This bit serves no function on the ADS1113.
	0b : Traditional comparator (default)
	1b : Window comparator
*/
enum {
	ADS1115_CONFIG_COMPARATOR_MODE = (1 << 4)
};

/*	Comparator polarity (ADS1114 and ADS1115 only)
	This bit controls the polarity of the ALERT/RDY pin.
	This bit serves no function on the ADS1113.
	0b : Active low (default)
	1b : Active high
*/
enum {
	ADS1115_CONFIG_COMPARATOR_ALERT_POLARITY = (1 << 3)
};

/*	Latching comparator (ADS1114 and ADS1115 only)
	This bit controls whether the ALERT/RDY pin latches after being asserted or clears after conversions are within the margin of the upper and lower threshold values.
	This bit serves no function on the ADS1113.
	0b : Nonlatching comparator. The ALERT/RDY pin does not latch when asserted (default).
	1b : Latching comparator. The asserted ALERT/RDY pin remains latched until conversion data are read by the controller or an appropriate SMBus alert
		response is sent by the controller. The device responds with an address, and is the lowest address currently asserting the ALERT/RDY bus line.
*/
enum {
	ADS1115_CONFIG_COMPARATOR_LATCHING = (1 << 2)
};

/*	Comparator queue and disable (ADS1114 and ADS1115 only)
	These bits perform two functions. When set to 11, the comparator is disabled and the ALERT/RDY pin is set to a high-impedance state. When set to any other
	value, the ALERT/RDY pin and the comparator function are enabled, and the set value determines the number of successive conversions exceeding the upper or
	lower threshold required before asserting the ALERT/RDY pin.
*/
enum {
	ADS1115_CONFIG_COMPARATOR_QUEUE_OFFSET	= 0,
	ADS1115_CONFIG_COMPARATOR_QUEUE_1 		= (0 << ADS1115_CONFIG_COMPARATOR_QUEUE_OFFSET),
	ADS1115_CONFIG_COMPARATOR_QUEUE_2 		= (1 << ADS1115_CONFIG_COMPARATOR_QUEUE_OFFSET),
	ADS1115_CONFIG_COMPARATOR_QUEUE_4 		= (2 << ADS1115_CONFIG_COMPARATOR_QUEUE_OFFSET),
	ADS1115_CONFIG_COMPARATOR_DISABLE		= (3 << ADS1115_CONFIG_COMPARATOR_QUEUE_OFFSET),	// sets ALERT/RDY pin to high impedance
};


}	// namespace

__fastcall TfrmCH341I2CAds1115::TfrmCH341I2CAds1115(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupAdc);

	for (int i=ADS1115_I2C_ADDRESS__START; i<ADS1115_I2C_ADDRESS__LIMIT; i++)
	{
		enum ADS1115_I2C_ADDRESS addr = static_cast<enum ADS1115_I2C_ADDRESS>(i);
		AnsiString text;
		text.sprintf("0x%02X (%s)", i, GetAds1115AddressDesc(addr));
		cbAddress->Items->Add(text);
	}
	cbAddress->ItemIndex = 0;

	cbMux->Items->Clear();
	for (unsigned int i=0; i<ARRAY_SIZE(mux); i++)
	{
		cbMux->Items->Add(mux[i].description);
	}
	cbMux->ItemIndex = 4;

	cbRange->Items->Clear();
	for (unsigned int i=0; i<ARRAY_SIZE(range); i++)
	{
		cbRange->Items->Add(range[i].description);
	}
	cbRange->ItemIndex = 2;

	cbRate->Items->Clear();
	for (unsigned int i=0; i<ARRAY_SIZE(rate); i++)
	{
		cbRate->Items->Add(rate[i].description);
	}
	cbRate->ItemIndex = 0;
}
//---------------------------------------------------------------------------

uint8_t TfrmCH341I2CAds1115::GetAddress(void)
{
	return static_cast<uint8_t>(ADS1115_I2C_ADDRESS__START + cbAddress->ItemIndex);
}


void __fastcall TfrmCH341I2CAds1115::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CAds1115::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	status = WriteRegister(ADS1115_REG_CONFIG,
		static_cast<uint16_t>(GetConfig() | ADS1115_CONFIG_OS));
	if (status != 0)
	{
		lblStatus->Caption = "I2C write failed!";
		return;
	}

	bool ready = false;
	for (int i=0; i<10; i++)
	{
		if (ConversionReady())
		{
			ready = true;
			break;
		}
		Sleep(50);
	}
	if (!ready)
	{
		lblStatus->Caption = "Timed out waiting for conversion ready flag!";
		return;
	}

	int16_t value = 0;
	status = ch341a.I2CWriteCommandReadWord(address, ADS1115_REG_CONVERSION, value);
	if (status != 0)
	{
		lblStatus->Caption = "I2C read failed!";
		return;
	}

	int16_t value2 = static_cast<int16_t>(((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8));

	// scale with range
	float voltPerBit = 0.0f;
	uint16_t rangeValue = range[cbRange->ItemIndex].value;
	switch (rangeValue)
	{
	case ADS1115_CONFIG_PGA_6144MV:
		voltPerBit = 6.144f / 32768;
		break;
	case ADS1115_CONFIG_PGA_4096MV:
		voltPerBit = 4.096f / 32768;
		break;
	case ADS1115_CONFIG_PGA_2048MV:
		voltPerBit = 2.048f / 32768;
		break;
	case ADS1115_CONFIG_PGA_1024MV:
		voltPerBit = 1.024f / 32768;
		break;
	case ADS1115_CONFIG_PGA_512MV:
		voltPerBit = 0.512f / 32768;
		break;
	case ADS1115_CONFIG_PGA_256MV:
	case ADS1115_CONFIG_PGA_256MV_2:
	case ADS1115_CONFIG_PGA_256MV_3:
		voltPerBit = 0.256f / 32768;
		break;
	default:
		assert(!"Unhandled range!");
		break;
	}

	AnsiString text;
	float voltage = voltPerBit * value2;
	text.sprintf("%d => %.4f V", static_cast<int>(value2), voltage);
	lblStatus->Caption = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CAds1115::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CAds1115::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CAds1115::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

bool TfrmCH341I2CAds1115::ConversionReady(void)
{
	uint16_t val = 0;
	int status = ReadRegister(ADS1115_REG_CONFIG, val);
	if (status == 0)
	{
		if (val & ADS1115_CONFIG_OS)
			return true;
		else
			return false;
	}
	else
	{
		lblStatus->Caption = "I2C operation failed!";
		return false;
	}
}

uint16_t TfrmCH341I2CAds1115::GetConfig(void)
{
	uint16_t val = 0;

	val |= mux[cbMux->ItemIndex].value;
	val |= range[cbRange->ItemIndex].value;
	val |= rate[cbRate->ItemIndex].value;
	val |= ADS1115_CONFIG_MODE_SINGLE;
		
	return val;
}

int TfrmCH341I2CAds1115::WriteRegister(uint8_t reg, uint16_t value)
{
	uint8_t buf[2];
	buf[0] = static_cast<uint8_t>((value & 0xFF00) >> 8);
	buf[1] = static_cast<uint8_t>(value & 0x00FF);
	LOG("Ads1115: Write 0x%02X 0x%02X to reg 0x%02X\n",
		static_cast<unsigned int>(buf[0]), static_cast<unsigned int>(buf[1]),
		static_cast<unsigned int>(reg));
	return ch341a.I2CWriteCommandWriteBytes(GetAddress(), reg, buf, sizeof(buf));
}

int TfrmCH341I2CAds1115::ReadRegister(uint8_t reg, uint16_t &value)
{
	value = 0;
	uint8_t buf[2];
	memset(buf, 0, sizeof(buf));
	int status = ch341a.I2CWriteCommandReadBytes(GetAddress(), reg, buf, sizeof(buf));
	if (status == 0)
	{
		value = static_cast<uint16_t>((static_cast<uint16_t>(buf[0]) << 8) | buf[1]);
		LOG("Ads1115: Read 0x%02X 0x%02X from reg 0x%02X\n",
			static_cast<unsigned int>(buf[0]), static_cast<unsigned int>(buf[1]),
			static_cast<unsigned int>(reg));
	}
	return status;
}




