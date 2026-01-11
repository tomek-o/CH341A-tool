//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CAHT20.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CAHT20 *frmCH341I2CAHT20;
//---------------------------------------------------------------------------

namespace
{

class AHT20
{
public:
	enum { ADDRESS = 0x38 };
private:

	enum Cmd {
		CMD_CALIBRATE = 0xE1,
		CMD_TRIGGER = 0xAC,
		CMD_SOFTRESET = 0xBA
	};

	void sleep(void)
	{
		for (unsigned int i=0; i<8; i++)
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
			// calibrate
			uint8_t buf[4];
			buf[0] = ADDRESS << 1;
			buf[1] = CMD_CALIBRATE;
			buf[2] = 0x08;
			buf[3] = 0x00;
			status = ch341a.I2CWriteRead(buf, sizeof(buf), NULL, 0);
			if (status != 0)
				return status;
		}

		Sleep(10);

		{
			// trigger measurement
			uint8_t buf[4];
			buf[0] = ADDRESS << 1;
			buf[1] = CMD_TRIGGER;
			buf[2] = 0x33;
			buf[3] = 0x00;
			status = ch341a.I2CWriteRead(buf, sizeof(buf), NULL, 0);
			if (status != 0)
				return status;
		}

		sleep();

		{
			// read data
			uint8_t buf[1];
			buf[0] = ADDRESS << 1;
			uint8_t data[6];
			status = ch341a.I2CWriteRead(buf, sizeof(buf), data, sizeof(data));
			if (status != 0)
				return status;

			uint32_t rawH = data[1];
			rawH <<= 8;
			rawH |= data[2];
			rawH <<= 4;
			rawH |= data[3] >> 4;
			humidity = static_cast<float>(rawH) * 100 / 0x100000;

			uint32_t rawT = data[3] & 0x0F;
			rawT <<= 8;
			rawT |= data[4];
			rawT <<= 8;
			rawT |= data[5];
			temperature = (static_cast<float>(rawT) * 200 / 0x100000) - 50;
		}

		return status;
	}

};

}	// namespace


__fastcall TfrmCH341I2CAHT20::TfrmCH341I2CAHT20(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CAHT20::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CAHT20::Clear(void)
{
	edTemperature->Text = "";
	edHumidity->Text = "";
}

void TfrmCH341I2CAHT20::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		Clear();
		return;
	}

	int status = ch341a.I2CCheckDev(AHT20::ADDRESS);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected I2C address!";
		Clear();
		return;
	}

	AHT20 sensor;

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
void __fastcall TfrmCH341I2CAHT20::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

