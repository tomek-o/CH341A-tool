//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CMAX30102.h"
#include "DFRobot_MAX30102.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/ScopedBool.h"
#include "Log.h"
#include "FormPlot.h"
#include <assert.h>
#include <algorithm>
#include <vector>
#include <stdlib.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CMAX30102 *frmCH341I2CMAX30102;
//---------------------------------------------------------------------------

namespace
{

DFRobot_MAX30102 sensor;

}

__fastcall TfrmCH341I2CMAX30102::TfrmCH341I2CMAX30102(TComponent* Owner)
	: TForm(Owner), reading(false), busy(false), hrRejectedCount(0)
{
	TabManager::Instance().Register(this, (1u << ToolGroupSensors));
	frmPlot1 = new TfrmPlot(pnlPlot1);
	frmPlot1->Parent = pnlPlot1;
	frmPlot1->Visible = true;
	frmPlot1->title = "RED";
	frmPlot2 = new TfrmPlot(pnlPlot2);
	frmPlot2->Parent = pnlPlot2;
	frmPlot2->Visible = true;
	frmPlot2->title = "INFRARED";
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMAX30102::btnStartClick(TObject *Sender)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	enum CH341AConf::I2CSpeed i2cSpeed = ch341a.GetI2CSpeed();
	AnsiString speedText;
	speedText.sprintf("I2C speed: %s", CH341AConf::getI2CSpeedDescription(i2cSpeed));
	if (i2cSpeed < CH341AConf::I2C_SPEED_100K)
	{
		speedText += ", 100+ kHz recommended!";
	}
	lblI2CSpeed->Caption = speedText;

	int status = ch341a.I2CCheckDev(MAX30102_IIC_ADDRESS);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}	

	if (!sensor.begin()) {
		lblStatus->Caption = "MAX30102 was not found";
		return;
	}

	// The sample rate is NOT free to choose: maxim_heart_rate_and_oxygen_saturation()
	// hardcodes the acquisition rate as FreqS (25 Hz) and derives the BPM from the
	// valley-to-valley distance measured in samples, so the FIFO output rate has to
	// be FreqS or the reported heart rate is wrong by exactly that ratio.
	// SAMPLERATE_100 / SAMPLEAVG_4 = 25 Hz, which is the rate the algorithm expects.
	sensor.sensorConfiguration(/*ledBrightness=*/150, /*sampleAverage=*/SAMPLEAVG_4, \
                        /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_100, \
						/*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_16384);

	lblStatus->Caption = "MAX30102 configured";
	hrHistory.clear();
	hrRejectedCount = 0;
	reading = true;
	lblReadingState->Caption = "Reading...";
	btnStart->Enabled = false;
	btnStop->Enabled = true;
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMAX30102::btnStopClick(TObject *Sender)
{
	reading = false;
	tmrAutoRead->Enabled = false;
	lblReadingState->Caption = "Stopped";
	btnStop->Enabled = false;
	btnStart->Enabled = true;
}
//---------------------------------------------------------------------------

int TfrmCH341I2CMAX30102::FilterHeartRate(int32_t heartRate, bool valid, AnsiString &note)
{
	enum {
		HR_MIN = 40,
		HR_MAX = 180,
		HISTORY_SIZE = 5,
		MAX_DEVIATION_PERCENT = 20,
		// After this many consecutive rejections the rate has most likely
		// really changed (e.g. after exercise) - restart instead of clinging
		// to the old median forever.
		MAX_CONSECUTIVE_REJECTS = 3
	};

	int median = -1;
	if (!hrHistory.empty())
	{
		std::vector<int> sorted(hrHistory.begin(), hrHistory.end());
		std::sort(sorted.begin(), sorted.end());
		median = sorted[sorted.size() / 2];
	}

	if (!valid)
	{
		note = ", rejected: invalid";
		return median;
	}
	if (heartRate < HR_MIN || heartRate > HR_MAX)
	{
		note = ", rejected: out of range";
		return median;
	}
	// Deviation check only once there is a meaningful median to compare with
	if (hrHistory.size() >= 3 &&
		abs(heartRate - median) * 100 > median * MAX_DEVIATION_PERCENT)
	{
		hrRejectedCount++;
		if (hrRejectedCount < MAX_CONSECUTIVE_REJECTS)
		{
			note = ", rejected: outlier";
			return median;
		}
		hrHistory.clear();
		note = ", history reset";
	}
	hrRejectedCount = 0;

	hrHistory.push_back(heartRate);
	while (hrHistory.size() > HISTORY_SIZE)
		hrHistory.pop_front();

	std::vector<int> sorted(hrHistory.begin(), hrHistory.end());
	std::sort(sorted.begin(), sorted.end());
	return sorted[sorted.size() / 2];
}

void TfrmCH341I2CMAX30102::Read(void)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	int32_t SPO2; //SPO2
	int8_t SPO2Valid; //Flag to display if SPO2 calculation is valid
	int32_t heartRate; //Heart-rate
	int8_t heartRateValid; //Flag to display if heart-rate calculation is valid
	uint32_t irBuffer[BUFFER_SIZE];
	uint32_t redBuffer[BUFFER_SIZE];

	uint32_t lostSamplesBefore = sensor.getFIFOOverflowCount();

	sensor.heartrateAndOxygenSaturation(irBuffer, redBuffer, BUFFER_SIZE,
		&SPO2, &SPO2Valid, &heartRate, &heartRateValid);

	uint32_t lostSamplesTotal = sensor.getFIFOOverflowCount();
	// Defensive: the counter could still only decrease if something else
	// reset it (e.g. Init) concurrently; avoid an unsigned-underflow wrap.
	uint32_t lostSamplesThisRead = (lostSamplesTotal >= lostSamplesBefore) ? (lostSamplesTotal - lostSamplesBefore) : 0;

	AnsiString hrNote;
	int hrFiltered = FilterHeartRate(heartRate, heartRateValid != 0, hrNote);

	AnsiString text;
	if (hrFiltered > 0)
		text.sprintf("Heart rate = %d BPM", hrFiltered);
	else
		text = "Heart rate = ---";
	if (SPO2Valid)
		text.cat_printf(", SPO2 = %d%%", SPO2);
	else
		text += ", SPO2 = ---";
	text.cat_printf("\n(raw: HR valid = %d, HR = %d, SPO2 valid = %d, SPO2 = %d%s)",
		(int)heartRateValid, heartRate, (int)SPO2Valid, SPO2, hrNote.c_str());
	if (lostSamplesTotal > 0)
	{
		text.cat_printf("\nWARNING: %u total total samples lost since Init",
			static_cast<unsigned int>(lostSamplesTotal));
		if (lostSamplesThisRead > 0)
			text.cat_printf(", %u lost THIS READ", static_cast<unsigned int>(lostSamplesThisRead));
	}
	lblStatus->Caption = text;

	frmPlot1->traces.clear();
	frmPlot1->traces.resize(1);
	{
		TfrmPlot::Trace &trace = frmPlot1->traces[0];
		trace.color = clRed;
		trace.samples = std::vector<int>(redBuffer, redBuffer + BUFFER_SIZE);
	}
	frmPlot1->DrawPlot();

	frmPlot2->traces.clear();
	frmPlot2->traces.resize(1);
	{
		TfrmPlot::Trace &trace = frmPlot2->traces[0];
		trace.color = clBlue;
		trace.samples = std::vector<int>(irBuffer, irBuffer + BUFFER_SIZE);
	}
	frmPlot2->DrawPlot();
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CMAX30102::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (reading)
	{
		lblReadingState->Caption = "Reading...";
		Read();
	}
	if (reading)
	{
		lblReadingState->Caption = "...";
		tmrAutoRead->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMAX30102::FormResize(TObject *Sender)
{
	pnlPlot1->Height = (Height - pnlPlot1->Top)/2 - 15;
	pnlPlot2->Height = (Height - pnlPlot1->Top)/2 - 15;	
	pnlPlot2->Top = pnlPlot1->Top + pnlPlot1->Height + 15;
}
//---------------------------------------------------------------------------

