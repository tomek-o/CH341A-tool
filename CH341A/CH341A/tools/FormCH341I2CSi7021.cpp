//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CSi7021.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CSi7021 *frmCH341I2CSi7021;
//---------------------------------------------------------------------------

namespace
{

class Si7021
{
public:
	enum { ADDRESS = 0x40 };
private:

	enum {
		CMD_MEASURE_RH = 0xE5,            			/**< Measure Relative Humidity, Hold Master Mode */
		CMD_MEASURE_RH_NO_HOLD = 0xF5,            	/**< Measure Relative Humidity, No Hold Master Mode */
		CMD_MEASURE_TEMP = 0xE3,		            /**< Measure Temperature, Hold Master Mode */
		CMD_MEASURE_TEMP_NO_HOLD = 0xF3,            /**< Measure Temperature, No Hold Master Mode */
		CMD_READ_TEMP = 0xE0,						/**< Read Temperature Value from Previous RH Measurement */
		CMD_RESET = 0xFE,				            /**< Reset */
		CMD_WRITE_USER_REG1 = 0xE6,		            /**< Write RH/T User Register 1 */
		CMD_READ_USER_REG1 = 0xE7,		            /**< Read RH/T User Register 1 */
		CMD_WRITE_HEATER_CTRL = 0x51,	            /**< Write Heater Control Register */
		CMD_READ_HEATER_CTRL = 0x11,	            /**< Read Heater Control Register */

		CMD_READ_ID_BYTE1A = 0xFA,
		CMD_READ_ID_BYTE1B = 0x0F,
		CMD_READ_ID_BYTE2A = 0xFC,
		CMD_READ_ID_BYTE2B = 0xC9,					/**< Read Electronic ID 2nd Byte */

		CMD_READ_FW_REV_BYTE1 = 0x84,
		CMD_READ_FW_REV_BYTE2 = 0xB8				/**< Read Firmware Revision */
	};

	int16_t read16(uint8_t reg)
	{
		int16_t data = 0xFFFF;
		int status = ch341a.I2CWriteCommandReadWord(ADDRESS, reg, data);
		(void)status;
		return data;
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

	void sleep(void)
	{
		for (unsigned int i=0; i<5; i++)
		{
			Sleep(10);
			Application->ProcessMessages();
		}
	}

public:
	int read(float &temperature, float &humidity)
	{
		int status;

		{
			// read and calculate temperature
			status = ch341a.I2CWriteByte(ADDRESS, 0xE3);
			if (status != 0)
				return status;

			sleep();

			uint8_t tempH = read8();
			uint8_t tempL = read8();

			float temp = (static_cast<unsigned int>(tempH) << 8) | tempL;
			temperature = ((175.72f * temp)/65536)-46.85f;
		}

		{
			// read and calculate humidity
			status = ch341a.I2CWriteByte(ADDRESS, 0xE5);
			if (status != 0)
				return status;

			sleep();

			uint8_t humH = read8();
			uint8_t humL = read8();

			float hum = (static_cast<unsigned int>(humH)<<8) | humL;
			humidity = ((125.0f*hum)/65536)-6;
		}

		return status;
	}

};


}	// namespace


__fastcall TfrmCH341I2CSi7021::TfrmCH341I2CSi7021(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CSi7021::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CSi7021::Clear(void)
{
	edTemperature->Text = "";
	edHumidity->Text = "";
}

void TfrmCH341I2CSi7021::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		Clear();
		return;
	}

	int status = ch341a.I2CCheckDev(Si7021::ADDRESS);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected Si7021 address!";
		Clear();
		return;
	}

	Si7021 sensor;

	float temperature = -9999, humidity = -9999;
	if (sensor.read(temperature, humidity) == 0)
	{
		lblStatus->Caption = "Read OK";
		AnsiString str;
		str.sprintf("%.2f C", temperature);
		edTemperature->Text = str;
		str.sprintf("%.2f%%", humidity);
		edHumidity->Text = str;
	}
	else
	{
		lblStatus->Caption = "Read error";
		Clear();
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CSi7021::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

