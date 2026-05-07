//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CSHT4x.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CSHT4x *frmCH341I2CSHT4x;
//---------------------------------------------------------------------------

namespace
{

enum {
	SHT4X_CMD_SOFT_RESET = 0x94,
	SHT4X_CMD_READ_SERIAL = 0x89,					///< serial = two 16-bit words, each one followed by 8-bit CRC
	SHT4X_CMD_MEASURE_HIGH_PRECISION = 0xFD,		///< measure temperature & RX with high precision/repeatability, [2 * 8-bit T-data; 8-bit CRC; 2 * 8-bit RH-data; 8-bit CRC]
	SHT4X_CMD_MEASURE_MEDIUM_PRECISION = 0xF6,
	SHT4X_CMD_MEASURE_LOW_PRECISION = 0xE0,
	SHT4X_CMD_MEASURE_WITH_HIGH_HEATER_1S = 0x39,	///< activate heater with 200mW for 1s, including a high precision measurement just before deactivation
	SHT4X_CMD_MEASURE_WITH_HIGH_HEATER_0S1 = 0x32,	///< activate heater for 0.1s and measure
	SHT4X_CMD_MEASURE_WITH_MEDIUM_HEATER_1S = 0x2F,	///< 110 mW heater
	SHT4X_CMD_MEASURE_WITH_MEDIUM_HEATER_0S1 = 0x24,
	SHT4X_CMD_MEASURE_WITH_LOW_HEATER_1S = 0x1E,	///< 20 mW heater
	SHT4X_CMD_MEASURE_WITH_LOW_HEATER_01S = 0x15,
};

int CheckCrc(uint8_t receivedCrc, uint8_t *data, int len)
{
	const uint8_t POLYNOMIAL = 0x31;
	uint8_t crc = 0xFF;
	for (int j = len; j != 0; --j)
	{
		crc ^= *data++;
		for (int i = 8; i != 0; --i)
		{
		#pragma warn -8071
			crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc<<1);
		#pragma warn .8071
		}
	}
	LOG("SHT4x CRC mismatch: received 0x%02X, calculated 0x%02X\n",
		static_cast<unsigned int>(receivedCrc), static_cast<unsigned int>(crc));
	return (crc == receivedCrc)?0:-1;                                                             /* return crc */
}

}	// namespace


__fastcall TfrmCH341I2CSHT4x::TfrmCH341I2CSHT4x(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CSHT4x::btnReadClick(TObject *Sender)
{
	Read(false);
}

void TfrmCH341I2CSHT4x::Clear(void)
{
	edTemperature->Text = "";
	edHumidity->Text = "";
}

void TfrmCH341I2CSHT4x::Read(bool heaterOn)
{
	BtnController btnCtrl(btnRead);
	BtnController btnCtrl2(btnReadWithHeater);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		Clear();
		return;
	}

	uint8_t i2cAddress = static_cast<uint8_t>(0x44 + cbAddress->ItemIndex);

	int status = ch341a.I2CCheckDev(i2cAddress);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending I2C address!";
		Clear();
		return;
	}

	/*
	Pseudocode from Sensirion datasheet:
	
	i2c_write(i2c_addr=0x44, tx_bytes=[0xFD])
	wait_seconds(0.01)
	rx_bytes = i2c_read(i2c_addr=0x44, number_of_bytes=6)
	t_ticks = rx_bytes[0] * 256 + rx_bytes[1]
	checksum_t = rx_bytes[2]
	rh_ticks = rx_bytes[3] * 256 + rx_bytes[4]
	checksum_rh = rx_bytes[5]
	t_degC = -45 + 175 * t_ticks/65535
	rh_pRH = -6 + 125 * rh_ticks/65535
	if (rh_pRH > 100):
		rh_pRH = 100
	if (rh_pRH < 0):
		rh_pRH = 0
	*/

	uint8_t measureCmd;
	int heaterDelay;
	if (heaterOn == false)
	{
		switch (cbPrecision->ItemIndex)
		{
		case 0:
			measureCmd = SHT4X_CMD_MEASURE_HIGH_PRECISION;
			break;
		case 1:
			measureCmd = SHT4X_CMD_MEASURE_MEDIUM_PRECISION;
			break;
		default:
			measureCmd = SHT4X_CMD_MEASURE_LOW_PRECISION;
			break;
		}
	}
	else
	{
		switch (cbHeater->ItemIndex)
		{
		case 0:
			measureCmd = SHT4X_CMD_MEASURE_WITH_HIGH_HEATER_1S;
			heaterDelay = 1100;
			break;
		case 1:
			measureCmd = SHT4X_CMD_MEASURE_WITH_HIGH_HEATER_0S1;
			heaterDelay = 110;
			break;
		case 2:
			measureCmd = SHT4X_CMD_MEASURE_WITH_MEDIUM_HEATER_1S;
			heaterDelay = 1100;
			break;
		case 3:
			measureCmd = SHT4X_CMD_MEASURE_WITH_MEDIUM_HEATER_0S1;
			heaterDelay = 110;
			break;
		case 4:
			measureCmd = SHT4X_CMD_MEASURE_WITH_LOW_HEATER_1S;
			heaterDelay = 1100;
			break;
		case 5:
			measureCmd = SHT4X_CMD_MEASURE_WITH_LOW_HEATER_01S;
			heaterDelay = 110;
			break;
		default:
			lblStatus->Caption = "Unhandled heater type!";
			Clear();
			return;
		}
	}

	status = ch341a.I2CWriteByte(i2cAddress, measureCmd);
	if (status)
	{
		lblStatus->Caption = "I2C write failed!";
		Clear();
		return;
	}

	if (!heaterOn)
	{
		Sleep(10);
	}
	else
	{
		while (heaterDelay > 0)
		{
			enum { PART = 50 };
			heaterDelay -= PART;
			Sleep(PART);
			Application->ProcessMessages();
		}
	}

	uint8_t rx_bytes[6];
	status = ch341a.I2CReadBytes(i2cAddress, rx_bytes, sizeof(rx_bytes));
	if (status)
	{
		lblStatus->Caption = "I2C read failed!";
		Clear();
		return;
	}

	if (CheckCrc(rx_bytes[2], rx_bytes + 0, 2) || CheckCrc(rx_bytes[5], rx_bytes + 3, 2))
	{
		lblStatus->Caption = "CRC mismatch in read data!";
		Clear();
		return;
	}

	int t_ticks = 256 * rx_bytes[0] + rx_bytes[1];
	//checksum_t = rx_bytes[2]
	int rh_ticks = 256 * rx_bytes[3] + rx_bytes[4];
	//checksum_rh = rx_bytes[5]
	float t_degC = -45.0f + 175.0f * t_ticks/65535;
	float rh_pRH = -6.0f + 125.0f * rh_ticks/65535;
	if (rh_pRH > 100)
		rh_pRH = 100;
	if (rh_pRH < 0)
		rh_pRH = 0;

	{
		lblStatus->Caption = "Read OK";
		AnsiString str;
		str.sprintf("%.2f C", t_degC);
		edTemperature->Text = str;
		str.sprintf("%.2f%%", rh_pRH);
		edHumidity->Text = str;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CSHT4x::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
	{
		Read(false);
		tmrAutoRead->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSHT4x::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSHT4x::chbAutoReadMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341I2CSHT4x::btnReadWithHeaterClick(TObject *Sender)
{
	bool tmrEnabled = tmrAutoRead->Enabled;
	tmrAutoRead->Enabled = false;
	Read(true);
	tmrAutoRead->Enabled = tmrEnabled;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSHT4x::btnReadSerialClick(TObject *Sender)
{
	BtnController btnCtrl(btnRead);
	edSerial->Text = "";

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t i2cAddress = static_cast<uint8_t>(0x44 + cbAddress->ItemIndex);

	int status = ch341a.I2CCheckDev(i2cAddress);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending I2C address!";
		return;
	}

	status = ch341a.I2CWriteByte(i2cAddress, SHT4X_CMD_READ_SERIAL);
	if (status)
	{
		lblStatus->Caption = "I2C write failed!";
		return;
	}

	Sleep(10);

	uint8_t rx_bytes[6];
	status = ch341a.I2CReadBytes(i2cAddress, rx_bytes, sizeof(rx_bytes));
	if (status)
	{
		lblStatus->Caption = "I2C read failed!";
		return;
	}

	if (CheckCrc(rx_bytes[2], rx_bytes + 0, 2) || CheckCrc(rx_bytes[5], rx_bytes + 3, 2))
	{
		lblStatus->Caption = "CRC mismatch in read data!";
		return;
	}

	AnsiString text;
	text.sprintf("0x%02X 0x%02X 0x%02X 0x%02X",
		static_cast<unsigned int>(rx_bytes[0]),
		static_cast<unsigned int>(rx_bytes[1]),
		static_cast<unsigned int>(rx_bytes[3]),
		static_cast<unsigned int>(rx_bytes[4]));
	edSerial->Text = text;
}
//---------------------------------------------------------------------------

