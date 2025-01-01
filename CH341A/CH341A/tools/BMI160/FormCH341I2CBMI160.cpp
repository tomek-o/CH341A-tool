//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CBMI160.h"
#include "bmi160.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CBMI160 *frmCH341I2CBMI160;
//---------------------------------------------------------------------------

namespace
{

enum
{
	ADDRESS_A =	0x68,
	ADDRESS_B =	0x69
};

/*! @brief This structure containing relevant bmi160 info */
struct bmi160_dev bmi160dev;

/*! @brief variable to hold the bmi160 accel data */
struct bmi160_sensor_data bmi160_accel;

/*! @brief variable to hold the bmi160 gyro data */
struct bmi160_sensor_data bmi160_gyro;

bool configured = false;

extern "C" void delay_msec(uint32_t period)
{
	Sleep(period);
}

int8_t read_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	return (int8_t)ch341a.I2CWriteCommandReadBytes(dev_addr, reg_addr, data, len);
}

int8_t write_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	return (int8_t)ch341a.I2CWriteCommandWriteBytes(dev_addr, reg_addr, data, len);
}


}

__fastcall TfrmCH341I2CBMI160::TfrmCH341I2CBMI160(TComponent* Owner)
	: TForm(Owner),
	dpsPerBit(0)
{
	TabManager::Instance().Register(this);

	bmi160dev.write = write_i2c;
	bmi160dev.read = read_i2c;
    bmi160dev.delay_ms = delay_msec;

    /* set correct i2c address */
    bmi160dev.id = ADDRESS_A;	
}
//---------------------------------------------------------------------------

uint8_t TfrmCH341I2CBMI160::GetAddress(void)
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

void __fastcall TfrmCH341I2CBMI160::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	Clear();
	configured = false;

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();
	bmi160dev.id = address;

	int8_t rslt = bmi160_init(&bmi160dev);

    if (rslt == BMI160_OK)
    {
		AnsiString text;
		text.sprintf("BMI160 initialized, chip ID = 0x%X", bmi160dev.chip_id);
		lblStatus->Caption = text;
    }
	else
	{
		lblStatus->Caption = "BMI160 initialization failed!";
	}

    /* Select the Output data rate, range of accelerometer sensor */
	bmi160dev.accel_cfg.odr = BMI160_ACCEL_ODR_1600HZ;
	uint8_t arange;
	switch (cbAccelerationRange->ItemIndex)
	{
		case 0:
			arange = BMI160_ACCEL_RANGE_2G;
			break;
		case 1:
			arange = BMI160_ACCEL_RANGE_4G;
			break;
		case 2:
			arange = BMI160_ACCEL_RANGE_8G;
			break;
		case 3:
			arange = BMI160_ACCEL_RANGE_16G;
			break;
		default:
			assert(!"Unhandled acceleration range!");
	}
	bmi160dev.accel_cfg.range = arange;
    bmi160dev.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;

    /* Select the power mode of accelerometer sensor */
    bmi160dev.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;

    /* Select the Output data rate, range of Gyroscope sensor */
	bmi160dev.gyro_cfg.odr = BMI160_GYRO_ODR_3200HZ;
	uint8_t grange;
	switch (cbGyroRange->ItemIndex)
	{
		case 0:
			grange = BMI160_GYRO_RANGE_125_DPS;
			break;
		case 1:
			grange = BMI160_GYRO_RANGE_250_DPS;
			break;
		case 2:
			grange = BMI160_GYRO_RANGE_500_DPS;
			break;
		case 3:
			grange = BMI160_GYRO_RANGE_1000_DPS;
			break;
		case 4:
			grange = BMI160_GYRO_RANGE_2000_DPS;
			break;
		default:
			assert(!"Unhandled gyro range!");
	}
	bmi160dev.gyro_cfg.range = grange;
    bmi160dev.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;

    /* Select the power mode of Gyroscope sensor */
    bmi160dev.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;

    /* Set the sensor configuration */
	rslt = bmi160_set_sens_conf(&bmi160dev);
	if (rslt != BMI160_OK)
	{
		lblStatus->Caption = "Failed to configure BMI160!";
	}
	else
	{
		lblStatus->Caption = "BMI160 initialized and configured";
		configured = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBMI160::btnReadClick(TObject *Sender)
{
	Read();
}

AnsiString TfrmCH341I2CBMI160::RawValToString(int val)
{
	AnsiString text = FloatToStrF(dpsPerBit * val, ffFixed, 6, 1);
	if (val >= 0)
		text = (AnsiString)" " + text;
	return text;
}

void TfrmCH341I2CBMI160::Clear(void)
{
	edAccelerationX->Text = "";
	edAccelerationY->Text = "";
	edAccelerationZ->Text = "";
	edGyroX->Text = "";
	edGyroY->Text = "";
	edGyroZ->Text = "";
}

void TfrmCH341I2CBMI160::Read(void)
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

	if (!configured)
	{
		lblStatus->Caption = "BMI 160 not initialized/configured!";
		Clear();
		return;
	}

	bmi160_get_sensor_data((BMI160_ACCEL_SEL | BMI160_GYRO_SEL), &bmi160_accel, &bmi160_gyro, &bmi160dev);

	edAccelerationX->Text = bmi160_accel.x;
	edAccelerationY->Text = bmi160_accel.y;
	edAccelerationZ->Text = bmi160_accel.z;
	edGyroX->Text = bmi160_gyro.x;
	edGyroY->Text = bmi160_gyro.y;
	edGyroZ->Text = bmi160_gyro.z;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CBMI160::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBMI160::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBMI160::chbAutoReadEnter(TObject *Sender)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

