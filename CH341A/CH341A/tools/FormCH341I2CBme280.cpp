//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CBme280.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CBme280 *frmCH341I2CBme280;
//---------------------------------------------------------------------------

namespace
{

class Bme280
{
public:
	enum { FIRST_ADDR = 0x76 };
	enum { LAST_ADDR = 0x77 };
private:
	enum {
		REGISTER_DIG_T1 = 0x88,			// calib00
		REGISTER_DIG_T2 = 0x8A,
		REGISTER_DIG_T3 = 0x8C,
		REGISTER_DIG_P1 = 0x8E,
		REGISTER_DIG_P2 = 0x90,
		REGISTER_DIG_P3 = 0x92,
		REGISTER_DIG_P4 = 0x94,
		REGISTER_DIG_P5 = 0x96,
		REGISTER_DIG_P6 = 0x98,
		REGISTER_DIG_P7 = 0x9A,
		REGISTER_DIG_P8 = 0x9C,
		REGISTER_DIG_P9 = 0x9E,
		REGISTER_DIG_H1 = 0xA1,
		REGISTER_DIG_H2 = 0xE1,
		REGISTER_DIG_H3 = 0xE3,
		REGISTER_DIG_H4 = 0xE4,
		REGISTER_DIG_H5 = 0xE5,
		REGISTER_DIG_H6 = 0xE7,
		REGISTER_CHIPID = 0xD0,
		REGISTER_SOFTRESET = 0xE0,
		REGISTER_CAL26 = 0xE1,			// calib26 ... calib41: 0xE1 ... 0xF0
		REGISTER_CONTROLHUMID = 0xF2,
		REGISTER_CONTROL = 0xF4,
		REGISTER_CONFIG = 0xF5,
		REGISTER_PRESSUREDATA = 0xF7,
		REGISTER_TEMPDATA = 0xFA,
		REGISTER_HUMIDDATA = 0xFD
	};

	enum { RESET = 0xB6 };

	struct Calibration
	{
		uint16_t T1;
		int16_t  T2;
		int16_t  T3;

		uint16_t P1;
		int16_t  P2;
		int16_t  P3;
		int16_t  P4;
		int16_t  P5;
		int16_t  P6;
		int16_t  P7;
		int16_t  P8;
		int16_t  P9;

		uint8_t  H1;
		int16_t  H2;
		uint8_t  H3;
		int16_t  H4;
		int16_t  H5;
		int8_t   H6;
	} cal;

	int32_t getTemperatureWithCalibration(int32_t adc_T) {
		int32_t var1  = ((((adc_T>>3) - ((int32_t)cal.T1 <<1))) * ((int32_t)cal.T2)) >> 11;
		int32_t var2  = (((((adc_T>>4) - ((int32_t)cal.T1)) * ((adc_T>>4) - ((int32_t)cal.T1))) >> 12) * ((int32_t)cal.T3)) >> 14;
		return var1 + var2;
	}

	float getCompensatedPressure(int32_t adc_P, int32_t t_fine) {
		int64_t var1, var2, p;

		var1 = static_cast<int64_t>(t_fine) - 128000;
		var2 = var1 * var1 * (int64_t)cal.P6;
		var2 = var2 + ((var1*(int64_t)cal.P5) << 17);
		var2 = var2 + (((int64_t)cal.P4) << 35);
		var1 = ((var1 * var1 * (int64_t)cal.P3)>>8) + ((var1 * (int64_t)cal.P2)<<12);
		var1 = (((((int64_t)1)<<47)+var1)) * ((int64_t)cal.P1)>>33;

		if (var1 == 0) {
			return 0;  // avoid division by zero
		}
		p = 1048576 - adc_P;
		p = (((p<<31) - var2)*3125) / var1;
		var1 = (((int64_t)cal.P9) * (p>>13) * (p>>13)) >> 25;
		var2 = (((int64_t)cal.P8) * p) >> 19;

		p = ((p + var1 + var2) >> 8) + (((int64_t)cal.P7)<<4);
		return (float)p/256;
	}


	float getCompensatedHumidity(int32_t adc_H, int32_t t_fine) {
		int32_t val;

		val = t_fine - ((int32_t)76800);

		val =
			((((adc_H << 14) - (((int32_t)cal.H4) << 20) - (((int32_t)cal.H5) * val)) + ((int32_t)16384)) >> 15) *
			(((((((val * ((int32_t)cal.H6)) >> 10) * (((val * ((int32_t)cal.H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)cal.H2) + 8192) >> 14);

		val = val - (((((val >> 15) * (val >> 15)) >> 7) * ((int32_t)cal.H1)) >> 4);

		val = (val < 0) ? 0 : val;
		val = (val > 419430400) ? 419430400 : val;

		return static_cast<float>(val >> 12) / 1024.0f;
	}

	int16_t read16(uint8_t reg)
	{
		int16_t data = 0xFFFF;
		int status = ch341a.I2CWriteCommandReadWord(address, reg, data);
		(void)status;
		return data;
	}

	uint8_t read8(uint8_t reg)
	{
		uint8_t data = 0xFF;
		int status = ch341a.I2CWriteCommandReadByte(address, reg, data);
		(void)status;
		return data;
	}

	uint8_t read8(void)
	{
		uint8_t data = 0xFF;
		int status = ch341a.I2CReadByte(address, data);
		(void)status;
		return data;
	}

	uint8_t address;

public:
	Bme280(uint8_t address):
		address(address)
	{}
	int readCalibration(void)
	{
		if (!ch341a.IsOpened())
		{
			return -1;
		}
		memset(&cal, 0xFF, sizeof(cal));
		
		cal.T1 = static_cast<uint16_t>(read16(REGISTER_DIG_T1));
		cal.T2 = read16(REGISTER_DIG_T2);
		cal.T3 = read16(REGISTER_DIG_T3);

		cal.P1 = static_cast<uint16_t>(read16(REGISTER_DIG_P1));
		cal.P2 = read16(REGISTER_DIG_P2);
		cal.P3 = read16(REGISTER_DIG_P3);
		cal.P4 = read16(REGISTER_DIG_P4);
		cal.P5 = read16(REGISTER_DIG_P5);
		cal.P6 = read16(REGISTER_DIG_P6);
		cal.P7 = read16(REGISTER_DIG_P7);
		cal.P8 = read16(REGISTER_DIG_P8);
		cal.P9 = read16(REGISTER_DIG_P9);

		cal.H1 = read8(REGISTER_DIG_H1);
		cal.H2 = read16(REGISTER_DIG_H2);
		cal.H3 = read8(REGISTER_DIG_H3);
	#ifdef __BORLANDC__
	#pragma warn -8071
	#endif
		cal.H4 = (static_cast<int16_t>(read8(REGISTER_DIG_H4)) << 4) | (read8(REGISTER_DIG_H4+1) & 0xF);
		cal.H5 = (static_cast<int16_t>(read8(REGISTER_DIG_H5+1)) << 4) | (read8(REGISTER_DIG_H5) >> 4);
	#ifdef __BORLANDC__
	#pragma warn .8071
	#endif
		cal.H6 = static_cast<int8_t>(read8(REGISTER_DIG_H6));

		return 0;
	}

	int setupRead(void)
	{
		int status;
		status = ch341a.I2CWriteCommandWriteByte(address, REGISTER_CONTROLHUMID, 0x01);
		if (status != 0)
			return status;
		ch341a.I2CWriteCommandWriteByte(address, REGISTER_CONTROL, 0x25);
		return status;
	}

	int read(float &temperature, float &pressure, float &humidity)
	{
		int status;
		status = ch341a.I2CWriteByte(address, REGISTER_PRESSUREDATA);
		if (status != 0)
			return status;

		uint32_t rawTemperature = 0;
		uint32_t rawPressure = 0;
		uint32_t rawHumidity = 0;

		{
			uint8_t pmsb = read8();
			uint8_t plsb = read8();
			uint8_t pxsb = read8();

			uint8_t tmsb = read8();
			uint8_t tlsb = read8();
			uint8_t txsb = read8();

			uint8_t hmsb = read8();
			uint8_t hlsb = read8();

			rawTemperature = (rawTemperature | tmsb) << 8;
			rawTemperature = (rawTemperature | tlsb) << 8;
			rawTemperature = (rawTemperature | txsb) >> 4;

			rawPressure = (rawPressure | pmsb) << 8;
			rawPressure = (rawPressure | plsb) << 8;
			rawPressure = (rawPressure | pxsb) >> 4;

			rawHumidity = (rawHumidity | hmsb) << 8;
			rawHumidity = (rawHumidity | hlsb);
		}

		int32_t t_fine = getTemperatureWithCalibration(rawTemperature);
		temperature = static_cast<float>((t_fine * 5 + 128) >> 8) / 100.0f;
		pressure = getCompensatedPressure(rawPressure, t_fine) / 100;
		humidity = getCompensatedHumidity(rawHumidity, t_fine);

		return status;
	}

};


}	// namespace


__fastcall TfrmCH341I2CBme280::TfrmCH341I2CBme280(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);

	for (unsigned int i=Bme280::FIRST_ADDR; i<=Bme280::LAST_ADDR; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbI2CAddress->Items->Add(text);
	}
	cbI2CAddress->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CBme280::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CBme280::Read(void)
{
	BtnController btnCtrl(btnRead);

	edTemperature->Text = "";
	edPressure->Text = "";
	edHumidity->Text = "";

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = static_cast<uint8_t>(Bme280::FIRST_ADDR + cbI2CAddress->ItemIndex);

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected BME280 address!";
		return;
	}

	Bme280 bme280(address);
	bme280.readCalibration();
	bme280.setupRead();

	float temperature = -9999, pressure = -9999, humidity = -9999;
	if (bme280.read(temperature, pressure, humidity) == 0)
	{
		lblStatus->Caption = "Read OK";
		AnsiString str;
		str.sprintf("%.2f C", temperature);
		edTemperature->Text = str;
		str.sprintf("%.2f hPa", pressure);
		edPressure->Text = str;
		str.sprintf("%.2f%%", humidity);
		edHumidity->Text = str;
	}
	else
	{
		lblStatus->Caption = "Read error";
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CBme280::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

