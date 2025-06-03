//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341Hmc5883L.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include "common/ArraySize.h"
#include "ValueDescription.h"
#include <assert.h>
#include <math.h>
#include <ClipBrd.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341Hmc5883L *frmCH341Hmc5883L;
//---------------------------------------------------------------------------

namespace
{

enum { HMC5883L_ADDRESS = 0x1E };

enum {
	REG_CONFIG_A = 0,
	REG_CONFIG_B,
	REG_MODE,
	REG_X_MSB,
	REG_X_LSB,
	REG_Z_MSB,
	REG_Z_LSB,
	REG_Y_MSB,
	REG_Y_LSB,
	REG_STATUS,
	REG_IDENTIFICATION_A,
	REG_IDENTIFICATION_B,
	REG_IDENTIFICATION_C
};

enum { CONFIG_A_SAMPLES_AVG_OFFSET = 5 };
const uint8_t CONFIG_A_SAMPLES_AVG_1 = 0x00 << CONFIG_A_SAMPLES_AVG_OFFSET;
const uint8_t CONFIG_A_SAMPLES_AVG_2 = 0x01 << CONFIG_A_SAMPLES_AVG_OFFSET;
const uint8_t CONFIG_A_SAMPLES_AVG_4 = 0x10 << CONFIG_A_SAMPLES_AVG_OFFSET;
const uint8_t CONFIG_A_SAMPLES_AVG_8 = 0x11 << CONFIG_A_SAMPLES_AVG_OFFSET;
ValueDescriptionU8 samplesAvgSel[] = {
	{ CONFIG_A_SAMPLES_AVG_1, "1 sample" },
	{ CONFIG_A_SAMPLES_AVG_2, "2 samples" },
	{ CONFIG_A_SAMPLES_AVG_4, "4 samples" },
	{ CONFIG_A_SAMPLES_AVG_8, "8 samples" },
};

enum { CONFIG_A_OUTPUT_RATE_OFFSET = 2 };
const uint8_t CONFIG_A_OUTPUT_RATE_0_75_HZ = 0x00 << CONFIG_A_OUTPUT_RATE_OFFSET;
const uint8_t CONFIG_A_OUTPUT_RATE_1_5_HZ = 0x01 << CONFIG_A_OUTPUT_RATE_OFFSET;
const uint8_t CONFIG_A_OUTPUT_RATE_3_HZ = 0x02 << CONFIG_A_OUTPUT_RATE_OFFSET;
const uint8_t CONFIG_A_OUTPUT_RATE_7_5_HZ = 0x03 << CONFIG_A_OUTPUT_RATE_OFFSET;
const uint8_t CONFIG_A_OUTPUT_RATE_15_HZ = 0x04 << CONFIG_A_OUTPUT_RATE_OFFSET;
const uint8_t CONFIG_A_OUTPUT_RATE_30_HZ = 0x05 << CONFIG_A_OUTPUT_RATE_OFFSET;
const uint8_t CONFIG_A_OUTPUT_RATE_75_HZ = 0x06 << CONFIG_A_OUTPUT_RATE_OFFSET;
ValueDescriptionU8 outputRateSel[] = {
	{ CONFIG_A_OUTPUT_RATE_0_75_HZ, "0.75 Hz" },
	{ CONFIG_A_OUTPUT_RATE_1_5_HZ, "1.5 Hz" },
	{ CONFIG_A_OUTPUT_RATE_3_HZ, "3 Hz" },
	{ CONFIG_A_OUTPUT_RATE_7_5_HZ, "7.5 Hz" },
	{ CONFIG_A_OUTPUT_RATE_15_HZ, "15 Hz" },
	{ CONFIG_A_OUTPUT_RATE_30_HZ, "30 Hz" },
	{ CONFIG_A_OUTPUT_RATE_75_HZ, "75 Hz" },
};

enum { CONFIG_A_MEAS_MODE_OFFSET = 0 };
const uint8_t CONFIG_A_MEAS_MODE_NORMAL = 0x00 << CONFIG_A_MEAS_MODE_OFFSET;
const uint8_t CONFIG_A_MEAS_MODE_POS_BIAS = 0x01 << CONFIG_A_MEAS_MODE_OFFSET;
const uint8_t CONFIG_A_MEAS_MODE_NEG_BIAS = 0x02 << CONFIG_A_MEAS_MODE_OFFSET;
ValueDescriptionU8 measModeSel[] = {
	{ CONFIG_A_MEAS_MODE_NORMAL, "normal / no bias" },
	{ CONFIG_A_MEAS_MODE_POS_BIAS, "positive bias" },
	{ CONFIG_A_MEAS_MODE_NEG_BIAS, "negative bias" },
};

enum { CONFIG_B_GAIN_OFFSET = 5 };
const uint8_t CONFIG_B_GAIN_0_88_GA = 0x00 << CONFIG_B_GAIN_OFFSET;
const uint8_t CONFIG_B_GAIN_1_3_GA = 0x01 << CONFIG_B_GAIN_OFFSET;
const uint8_t CONFIG_B_GAIN_1_9_GA = 0x02 << CONFIG_B_GAIN_OFFSET;
const uint8_t CONFIG_B_GAIN_2_5_GA = 0x03 << CONFIG_B_GAIN_OFFSET;
const uint8_t CONFIG_B_GAIN_4_GA = 0x04 << CONFIG_B_GAIN_OFFSET;
const uint8_t CONFIG_B_GAIN_4_7_GA = 0x05 << CONFIG_B_GAIN_OFFSET;
const uint8_t CONFIG_B_GAIN_5_6_GA = 0x06 << CONFIG_B_GAIN_OFFSET;
const uint8_t CONFIG_B_GAIN_8_1_GA = 0x07 << CONFIG_B_GAIN_OFFSET;
ValueDescriptionU8 gainSel[] = {
	{ CONFIG_B_GAIN_0_88_GA, "+/-0.88 Gs" },
	{ CONFIG_B_GAIN_1_3_GA, "+/-1.3 Gs" },
	{ CONFIG_B_GAIN_1_9_GA, "+/-1.9 Gs" },
	{ CONFIG_B_GAIN_2_5_GA, "+/-2.5 Gs" },
	{ CONFIG_B_GAIN_4_GA, "+/-4 Gs" },
	{ CONFIG_B_GAIN_4_7_GA, "+/-4.7 Gs" },
	{ CONFIG_B_GAIN_5_6_GA, "+/-5.6 Gs" },
	{ CONFIG_B_GAIN_8_1_GA, "+/- 8.1 Gs" },
};

enum { MODE_OFFSET = 0 };
const uint8_t MODE_CONTINUOUS = 0x00 << MODE_OFFSET;
const uint8_t MODE_SINGLE = 0x01 << MODE_OFFSET;
const uint8_t MODE_IDLE = 0x10 << MODE_OFFSET;

const int16_t OVERFLOW_VALUE = -4096;


int16_t swap_int16( int16_t val )
{
#ifdef __BORLANDC__
#pragma warn -8071
#endif
	return (val << 8) | ((val >> 8) & 0xFF);
#ifdef __BORLANDC__
#pragma warn .8071
#endif
}

}	// namespace

__fastcall TfrmCH341Hmc5883L::TfrmCH341Hmc5883L(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
	FillComboboxWithValues(samplesAvgSel, cbAvgSel, CONFIG_A_SAMPLES_AVG_8);
	FillComboboxWithValues(outputRateSel, cbOutputRateSel, CONFIG_A_OUTPUT_RATE_15_HZ);
	FillComboboxWithValues(gainSel, cbGainSel, CONFIG_B_GAIN_1_3_GA);
	FillComboboxWithValues(measModeSel, cbMeasModeSel, CONFIG_A_MEAS_MODE_NORMAL);

}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Hmc5883L::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status;

	status = ch341a.I2CWriteCommandWriteByte(HMC5883L_ADDRESS, REG_CONFIG_A,
		samplesAvgSel[cbAvgSel->ItemIndex].value |
		outputRateSel[cbOutputRateSel->ItemIndex].value |
		measModeSel[cbMeasModeSel->ItemIndex].value
	);
	if (status != 0)
	{
		lblStatus->Caption = "Error writing REG_CONFIG_A!";
		return;
	}

	status = ch341a.I2CWriteCommandWriteByte(HMC5883L_ADDRESS, REG_CONFIG_B,
		gainSel[cbGainSel->ItemIndex].value
	);
	if (status != 0)
	{
		lblStatus->Caption = "Error writing REG_CONFIG_B!";
		return;
	}

	status = ch341a.I2CWriteCommandWriteByte(HMC5883L_ADDRESS, REG_MODE,
		MODE_CONTINUOUS
	);
	if (status != 0)
	{
		lblStatus->Caption = "Error writing REG_CONFIG_B!";
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Hmc5883L::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341Hmc5883L::Read(void)
{
	BtnController btnCtrl(btnRead);


	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(HMC5883L_ADDRESS);
	if (status != 0)
	{
		AnsiString text;
		text.sprintf("No ACK after sending expected address (0x%02X)!", HMC5883L_ADDRESS);
		lblStatus->Caption = text;
		return;
	}

	int16_t data[3];
	status = ch341a.I2CWriteCommandReadBytes(HMC5883L_ADDRESS, REG_X_MSB, reinterpret_cast<uint8_t*>(data), sizeof(data));
	if (status != 0)
	{
		lblStatus->Caption = "I2C operation failed";
		return;
	}
	for (unsigned int i=0; i<ARRAY_SIZE(data); i++)
	{
		data[i] = swap_int16(data[i]);
	}

	edRawX->Text = data[0];
	edRawY->Text = data[2];
	edRawZ->Text = data[1];

	lblStatus->Caption = "";

	int offsetX = StrToIntDef(edOffsetX->Text, 0);
	int offsetY = StrToIntDef(edOffsetY->Text, 0);
	int offsetZ = StrToIntDef(edOffsetZ->Text, 0);
	int x, y, z;

	double lsb = GetGaussPerLsb();
	if (data[0] != OVERFLOW_VALUE)
	{
		x = data[0] - offsetX;
		AnsiString text;
		text.sprintf("%.3f", lsb * x);
		edMagneticInductionX->Text = text;
	}
	else
	{
		edMagneticInductionX->Text = "";
	}

	if (data[2] != OVERFLOW_VALUE)
	{
		y = data[2] - offsetY;
		AnsiString text;
		text.sprintf("%.3f", lsb * y);
		edMagneticInductionY->Text = text;
	}
	else
	{
		edMagneticInductionY->Text = "";
	}

	if (data[0] != OVERFLOW_VALUE && data[2] != OVERFLOW_VALUE)
	{
		double heading = atan2(y, x);
		if (heading < 0)
			heading += (M_PI * 2.0);
		if (heading > (M_PI * 2.0))
			heading -= (M_PI * 2.0);
		double degrees = heading * 180 / M_PI;
		edHeadingDegrees->Text = static_cast<int>(degrees);
	}
	else
	{
		edHeadingDegrees->Text = "";
	}

	if (data[1] != OVERFLOW_VALUE)
	{
		z = data[1] - offsetZ;
		AnsiString text;
		text.sprintf("%.3f", lsb * z);
		edMagneticInductionZ->Text = text;
	}
	else
	{
		edMagneticInductionZ->Text = "";
	}

	if (chbCalibrationCollect->Checked)
	{
		DataPoint dp;
		dp.X = data[0];
		dp.Y = data[2];
		dp.Z = data[1];
		dataPoints.push_back(dp);
		UpdateDataStats();
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341Hmc5883L::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Hmc5883L::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Hmc5883L::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

void TfrmCH341Hmc5883L::UpdateDataStats(void)
{
	AnsiString text;
	text.sprintf("Data points: %u", dataPoints.size());
	lblPointsCount->Caption = text;
	if (!dataPoints.empty())
	{
		int minX = INT_MAX;
		int maxX = INT_MIN;
		int minY = INT_MAX;
		int maxY = INT_MIN;
		for (unsigned int i=0; i<dataPoints.size(); i++)
		{
			const DataPoint &dp = dataPoints[i];
			if (minX > dp.X)
				minX = dp.X;
			if (maxX < dp.X)
				maxX = dp.X;
			if (minY > dp.Y)
				minY = dp.Y;
			if (maxY < dp.Y)
				maxY = dp.Y;
		}
		edMiddleX->Text = (minX + maxX)/2;
		edMiddleY->Text = (minY + maxY)/2;
	}
	else
	{
		edMiddleX->Text = "";
		edMiddleY->Text = "";
	}
}

void __fastcall TfrmCH341Hmc5883L::btnResetDataClick(TObject *Sender)
{
	dataPoints.clear();
	UpdateDataStats();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Hmc5883L::btnCopyDataClick(TObject *Sender)
{
	AnsiString text;
	text.cat_printf("X,Y,Z\r\n");
	for (unsigned int i=0; i<dataPoints.size(); i++)
	{
		const DataPoint &dp = dataPoints[i];
		text.cat_printf("%d,%d,%d\r\n", dp.X, dp.Y, dp.Z);
	}
	Clipboard()->SetTextBuf(text.c_str());
}
//---------------------------------------------------------------------------

double TfrmCH341Hmc5883L::GetGaussPerLsb(void)
{
	switch (gainSel[cbGainSel->ItemIndex].value)
	{
	case CONFIG_B_GAIN_0_88_GA:
		return 1.0/1370;
	case CONFIG_B_GAIN_1_3_GA:
		return 1.0/1090;
	case CONFIG_B_GAIN_1_9_GA:
		return 1.0/820;
	case CONFIG_B_GAIN_2_5_GA:
		return 1.0/660;
	case CONFIG_B_GAIN_4_GA:
		return 1.0/440;
	case CONFIG_B_GAIN_4_7_GA:
		return 1.0/390;
	case CONFIG_B_GAIN_5_6_GA:
		return 1.0/330;
	case CONFIG_B_GAIN_8_1_GA:
		return 1.0/230;
	default:
		return 0.0;
	}
}

