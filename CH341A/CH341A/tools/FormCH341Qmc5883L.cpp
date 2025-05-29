//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341Qmc5883L.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341Qmc5883L *frmCH341Qmc5883L;
//---------------------------------------------------------------------------

namespace
{

enum { QMC5883L_ADDR = 0x0D };	// unlikely to need configuration: "If other I2C address options are required, please contact factory."

/*	The I2C data pointer will automatically roll between 00H ~ 06H if I2C read begins at any position among 00H~06H.
	This function is enabled by set 0AH[6] = 01H.
*/

enum {
	REG_X_LSB = 0x00,	// Read only
	REG_X_MSB = 0x01,	// Read only
	REG_Y_LSB = 0x02,	// Read only
	REG_Y_MSB = 0x03,	// Read only
	REG_Z_LSB = 0x04,	// Read only
	REG_Z_MSB = 0x05,	// Read only
	REG_STATUS = 0x06,	// Read only
		STATUS_DATA_READY = (1<<0),		// data from all the axis ready in continuous mode
		STATUS_OVERFLOW = (1<<1),
		STATUS_DATA_SKIP = (1<<2),
	REG_TEMPERATURE_LSB = 0x07,			// 100 LSB / deg C but without offset calibration, only relative temperature is accurate
	REG_TEMPERATURE_MSB = 0x08,
	REG_CONTROL_1 = 0x09,
		CONTROL_1_MODE_STANDBY = (0x00 << 0),
		CONTROL_1_MODE_CONTINUOUS = (0x01 << 0),
		CONTROL_1_ODR_10 = (0x00 << 2),	// output data rate = 10 Hz
		CONTROL_1_ODR_50 = (0x01 << 2),
		CONTROL_1_ODR_100 = (0x02 << 2),
		CONTROL_1_ODR_200 = (0x03 << 2),
		CONTROL_1_RANGE_2_GAUSS = (0x00 << 4),
		CONTROL_1_RANGE_8_GAUSS = (0x01 << 4),
		CONTROL_1_OVERSAMPLE_512 = (0x00 << 6),
		CONTROL_1_OVERSAMPLE_256 = (0x01 << 6),
		CONTROL_1_OVERSAMPLE_128 = (0x02 << 6),
		CONTROL_1_OVERSAMPLE_64 = (0x03 << 6),
	REG_CONTROL_2 = 0x0A,
		CONTROL_2_INTERRUPT_ENABLE = 0x01,
		CONTROL_2_POINTER_ROLLOVER = 0x40,
		CONTROL_2_SOFT_RESET = 0x80,
	REG_SET_RESET_PERIOD = 0x0B,	// It is recommended that the register 0BH is written by 0x01.
	// 0x0C: reserved or status? datasheet is inconsistent
	REG_CHIP_ID = 0x0D,				// returns 0xFF (useless?)
};

}	// namespace


__fastcall TfrmCH341Qmc5883L::TfrmCH341Qmc5883L(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Qmc5883L::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}


}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Qmc5883L::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341Qmc5883L::Read(void)
{
	BtnController btnCtrl(btnRead);


	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(QMC5883L_ADDR);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}


}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341Qmc5883L::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Qmc5883L::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Qmc5883L::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

