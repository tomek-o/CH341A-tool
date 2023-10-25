//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CBme280.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CBme280 *frmCH341I2CBme280;
//---------------------------------------------------------------------------

namespace
{

class Bme280
{
private:
	enum { ADDRESS = 0x76 };

	enum {
		REGISTER_DIG_T1 = 0x88,
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
		REGISTER_VERSION = 0xD1,
		REGISTER_SOFTRESET = 0xE0,
		REGISTER_CAL26 = 0xE1,
		REGISTER_CONTROLHUMID = 0xF2,
		REGISTER_CONTROL = 0xF4,
		REGISTER_CONFIG = 0xF5,
		REGISTER_PRESSUREDATA = 0xF7,
		REGISTER_TEMPDATA = 0xFA,
		REGISTER_HUMIDDATA = 0xFD
	};

	enum { RESET = 0xB6 };

	struct CalibrationData
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

public:
	int readCalibration(void);
	
};


}	// namespace


__fastcall TfrmCH341I2CBme280::TfrmCH341I2CBme280(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CBme280::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CBme280::Read(void)
{
	BtnController btnCtrl(btnRead);
	if (!ch341a.IsOpened())
	{
		memo->Text = "CH341 is not opened!";
		return;
	}
	memo->Clear();
	AnsiString text;

	memo->Text = text;
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

