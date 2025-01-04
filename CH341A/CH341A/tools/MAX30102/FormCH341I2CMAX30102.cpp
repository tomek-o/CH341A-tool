//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CMAX30102.h"
#include "DFRobot_MAX30102.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include "FormPlot.h"
#include <assert.h>
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
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
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

void __fastcall TfrmCH341I2CMAX30102::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	if (!sensor.begin()) {
		lblStatus->Caption = "MAX30102 was not found";
		return;
	}

	sensor.sensorConfiguration(/*ledBrightness=*/150, /*sampleAverage=*/SAMPLEAVG_4, \
                        /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_100, \
						/*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_16384);

	lblStatus->Caption = "MAX30102 configured";
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMAX30102::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CMAX30102::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	int status = ch341a.I2CCheckDev(MAX30102_IIC_ADDRESS);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	int32_t SPO2; //SPO2
	int8_t SPO2Valid; //Flag to display if SPO2 calculation is valid
	int32_t heartRate; //Heart-rate
	int8_t heartRateValid; //Flag to display if heart-rate calculation is valid
	enum { BUF_SIZE = 100 };
	uint32_t irBuffer[BUF_SIZE];
	uint32_t redBuffer[BUF_SIZE];

	sensor.heartrateAndOxygenSaturation(irBuffer, redBuffer, BUF_SIZE,
		&SPO2, &SPO2Valid, &heartRate, &heartRateValid);


	AnsiString text;
	text.sprintf("SPO2 valid = %d, SPO2 = %d, heart rate valid = %d, heart rate = %d",
		(int)SPO2Valid, SPO2, (int)heartRateValid, heartRate);
	lblStatus->Caption = text;

	frmPlot1->traces.clear();
	frmPlot1->traces.resize(1);
	{
		TfrmPlot::Trace &trace = frmPlot1->traces[0];
		trace.color = clRed;
		trace.samples = std::vector<int>(redBuffer, redBuffer + BUF_SIZE);
	}
	frmPlot1->DrawPlot();

	frmPlot2->traces.clear();
	frmPlot2->traces.resize(1);
	{
		TfrmPlot::Trace &trace = frmPlot2->traces[0];
		trace.color = clBlue;
		trace.samples = std::vector<int>(irBuffer, irBuffer + BUF_SIZE);
	}
	frmPlot2->DrawPlot();
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CMAX30102::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMAX30102::FormResize(TObject *Sender)
{
	pnlPlot1->Height = (Height - pnlPlot1->Top)/2 - 15;
	pnlPlot2->Height = (Height - pnlPlot1->Top)/2 - 15;	
	pnlPlot2->Top = pnlPlot1->Top + pnlPlot1->Height + 15;
}
//---------------------------------------------------------------------------

