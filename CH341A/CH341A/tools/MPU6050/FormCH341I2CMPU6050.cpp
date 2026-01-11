//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CMPU6050.h"
#include "MPU6050.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CMPU6050 *frmCH341I2CMPU6050;
//---------------------------------------------------------------------------

namespace
{

MPU6050 mpu;

enum
{
	ADDRESS_A =	0x68,
	ADDRESS_B =	0x69
};

}

__fastcall TfrmCH341I2CMPU6050::TfrmCH341I2CMPU6050(TComponent* Owner)
	: TForm(Owner),
	dpsPerBit(0)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);
}
//---------------------------------------------------------------------------

uint8_t TfrmCH341I2CMPU6050::GetAddress(void)
{
	switch (cbAddress->ItemIndex)
	{
	case 0:
		return ADDRESS_A;
	case 1:
		return ADDRESS_B;
	default:
		assert(!"Unhandled address!");
		return 0;
	}
}

void __fastcall TfrmCH341I2CMPU6050::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();
	mpu.setDevAddr(address);

	mpu.initialize();

	uint8_t arange;
	switch (cbAccelerationRange->ItemIndex)
	{
		case 0:
			arange = MPU6050_ACCEL_FS_2;
			break;
		case 1:
			arange = MPU6050_ACCEL_FS_4;
			break;
		case 2:
			arange = MPU6050_ACCEL_FS_8;
			break;
		case 3:
			arange = MPU6050_ACCEL_FS_16;
			break;
		default:
			assert(!"Unhandled acceleration range!");
	}
	mpu.setFullScaleAccelRange(arange);

	uint8_t grange;
	switch (cbGyroRange->ItemIndex)
	{
		case 0:
			grange = MPU6050_GYRO_FS_250;
			break;
		case 1:
			grange = MPU6050_GYRO_FS_500;
			break;
		case 2:
			grange = MPU6050_GYRO_FS_1000;
			break;
		case 3:
			grange = MPU6050_GYRO_FS_2000;
			break;
		default:
			assert(!"Unhandled gyro range!");
	}
    mpu.setFullScaleGyroRange(grange);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMPU6050::btnReadClick(TObject *Sender)
{
	Read();
}

AnsiString TfrmCH341I2CMPU6050::RawValToString(int val)
{
	AnsiString text = FloatToStrF(dpsPerBit * val, ffFixed, 6, 1);
	if (val >= 0)
		text = (AnsiString)" " + text;
	return text;
}

void TfrmCH341I2CMPU6050::Clear(void)
{
	edAccelerationX->Text = "";
	edAccelerationY->Text = "";
	edAccelerationZ->Text = "";
	edGyroX->Text = "";
	edGyroY->Text = "";
	edGyroZ->Text = "";
}

void TfrmCH341I2CMPU6050::Read(void)
{
	BtnController btnCtrl(btnRead);


	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		Clear();
		return;
	}

	uint8_t address = GetAddress();

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		Clear();
		return;
	}

	int16_t ax, ay, az;
	int16_t gx, gy, gz;

	int rc = mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	if (rc == 0)
	{
		edAccelerationX->Text = ax;
		edAccelerationY->Text = ay;
		edAccelerationZ->Text = az;
		edGyroX->Text = gx;
		edGyroY->Text = gy;
		edGyroZ->Text = gz;
		lblStatus->Caption = "";
	}
	else
	{
		lblStatus->Caption = "Error reading data from sensor!";
		Clear();
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CMPU6050::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------





void __fastcall TfrmCH341I2CMPU6050::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMPU6050::chbAutoReadEnter(TObject *Sender)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

