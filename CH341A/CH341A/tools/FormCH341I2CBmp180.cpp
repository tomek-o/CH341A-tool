//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CBmp180.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CBmp180 *frmCH341I2CBmp180;
//---------------------------------------------------------------------------

namespace
{

class Bmp180
{
public:
	enum { ADDRESS = 0x77 };
private:
	enum {
		REGISTER_ID = 0xD0,
		REGISTER_CONTROL = 0xF4,

		REGISTER_CALIBRATION_START = 0xAA,
		REGISTER_CALIBRATION_END = 0xBE,

		REGISTER_CAL_AC1 = 0xAA,
		REGISTER_CAL_AC2 = 0xAC,
		REGISTER_CAL_AC3 = 0xAE,
		REGISTER_CAL_AC4 = 0xB0,
		REGISTER_CAL_AC5 = 0xB2,
		REGISTER_CAL_AC6 = 0xB4,
		REGISTER_CAL_B1 = 0xB6,
		REGISTER_CAL_B2 = 0xB8,
		REGISTER_CAL_MB	= 0xBA,
		REGISTER_CAL_MC = 0xBC,
		REGISTER_CAL_MD	= 0xBE,

		REGISTER_OUT_MSB = 0xF6,
		REGISTER_OUT_LSB = 0xF7,
		REGISTER_OUT_XLSB = 0xF8,			///< ultra high resolution, extending to 19 bits
		REGISTER_SOFT_RESET = 0xE0,
	};

	enum {
		SOFT_RESET = 0xB6
	};

	enum {
		CONTROL_MEASURE_TEMPERATURE = 0x2E,
		CONTROL_MEASURE_PRESSURE_OSS0 = 0x34,
		CONTROL_MEASURE_PRESSURE_OSS1 = 0x74,	// oversampling: 2x
		CONTROL_MEASURE_PRESSURE_OSS2 = 0xB4,	// oversampling: 4x
		CONTROL_MEASURE_PRESSURE_OSS3 = 0xF4	// oversampling: 8x
	};

	struct Calibration
	{
		int16_t AC1;
		int16_t AC2;
		int16_t AC3;
		uint16_t AC4;
		uint16_t AC5;
		uint16_t AC6;
		int16_t B1;
		int16_t B2;
		int16_t MB;
		int16_t MC;
		int16_t MD;
	} cal;

	uint16_t swap_uint16( uint16_t val )
	{
		return ((val & 0xFF) << 8) | (val >> 8 );
	}

	int16_t swap_int16( int16_t val )
	{
		return (val << 8) | ((val >> 8) & 0xFF);
	}

	int16_t readi16(uint8_t reg)
	{
		int16_t data = 0xFFFF;
		int status = ch341a.I2CWriteCommandReadWord(ADDRESS, reg, data);
		(void)status;
		data = swap_int16(data);
		return data;
	}

	uint16_t readu16(uint8_t reg)
	{
		union
		{
			int16_t data;
			uint16_t udata;
		} u;
		u.data = 0xFFFF;
		int status = ch341a.I2CWriteCommandReadWord(ADDRESS, reg, u.data);
		(void)status;
		u.udata = swap_uint16(u.udata);
		return u.udata;
	}

	uint8_t read8(uint8_t reg)
	{
		uint8_t data = 0xFF;
		int status = ch341a.I2CWriteCommandReadByte(ADDRESS, reg, data);
		(void)status;
		return data;
	}

	uint8_t read8(void)
	{
		uint8_t data = 0xFF;
		int status = ch341a.I2CReadByte(ADDRESS, data);
		(void)status;
		return data;
	}

public:
	int readCalibration(void)
	{
		if (!ch341a.IsOpened())
		{
			return -1;
		}
		memset(&cal, 0xFF, sizeof(cal));

		cal.AC1 = readi16(REGISTER_CAL_AC1);
		cal.AC2 = readi16(REGISTER_CAL_AC2);
		cal.AC3 = readi16(REGISTER_CAL_AC3);
		cal.AC4 = readu16(REGISTER_CAL_AC4);
		cal.AC5 = readu16(REGISTER_CAL_AC5);
		cal.AC6 = readu16(REGISTER_CAL_AC6);
		cal.B1 = readi16(REGISTER_CAL_B1);
		cal.B2 = readi16(REGISTER_CAL_B2);
		cal.MB = readi16(REGISTER_CAL_MB);
		cal.MC = readi16(REGISTER_CAL_MC);
		cal.MD = readi16(REGISTER_CAL_MD);

		return 0;
	}

	int read(float &temperature, float &pressure)
	{
		int status;
		//status = ch341a.I2CWriteByte(ADDRESS, REGISTER_PRESSUREDATA);
		//if (status != 0)
		//	return status;

		uint32_t rawTemperature = 0;
		uint32_t rawPressure = 0;


		return status;
	}

};


}	// namespace


__fastcall TfrmCH341I2CBmp180::TfrmCH341I2CBmp180(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CBmp180::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CBmp180::Read(void)
{
	BtnController btnCtrl(btnRead);

	edTemperature->Text = "";
	edPressure->Text = "";

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(Bmp180::ADDRESS);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected BMP180 address!";
		return;
	}

	Bmp180 bmp180;
	bmp180.readCalibration();

	float temperature = -9999, pressure = -9999;
	if (bmp180.read(temperature, pressure) == 0)
	{
		lblStatus->Caption = "Read OK";
		AnsiString str;
		str.sprintf("%.2f C", temperature);
		edTemperature->Text = str;
		str.sprintf("%.2f hPa", pressure);
		edPressure->Text = str;
	}
	else
	{
		lblStatus->Caption = "Read error";
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CBmp180::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

