//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CVl53l0x.h"
#include "VL53L0X.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CVl53l0x *frmCH341I2CVl53l0x;
//---------------------------------------------------------------------------

namespace
{
	VL53L0X sensor;
}


// Uncomment this line to use long range mode. This
// increases the sensitivity of the sensor and extends its
// potential range, but increases the likelihood of getting
// an inaccurate reading because of reflections from objects
// other than the intended target. It works best in dark
// conditions.

#define LONG_RANGE


// Uncomment ONE of these two lines to get
// - higher speed at the cost of lower accuracy OR
// - higher accuracy at the cost of lower speed

//#define HIGH_SPEED
//#define HIGH_ACCURACY


__fastcall TfrmCH341I2CVl53l0x::TfrmCH341I2CVl53l0x(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CVl53l0x::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CVl53l0x::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		edDistance->Text = "";
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	sensor.setTimeout(500);
	if (!sensor.init())
	{
		edDistance->Text = "";
		lblStatus->Caption = "Failed to detect/init sensor!";
		return;
	}

#if defined LONG_RANGE
	// lower the return signal rate limit (default is 0.25 MCPS)
	sensor.setSignalRateLimit(0.1f);
	// increase laser pulse periods (defaults are 14 and 10 PCLKs)
	sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
	sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
	// reduce timing budget to 20 ms (default is about 33 ms)
	sensor.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
	// increase timing budget to 200 ms
	sensor.setMeasurementTimingBudget(200000);
#endif

	edDistance->Text = sensor.readRangeSingleMillimeters();
	if (sensor.timeoutOccurred())
	{
		edDistance->Text = "";
		lblStatus->Caption = "TIMEOUT!";
	}
	else
	{
    	lblStatus->Caption = "";
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CVl53l0x::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

