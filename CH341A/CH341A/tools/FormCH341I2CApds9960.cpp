//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CApds9960.h"
#include "CH341A.h"
#include "SparkFun_APDS9960.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <time.h>
#include <sys\timeb.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CApds9960 *frmCH341I2CApds9960;
//---------------------------------------------------------------------------

namespace
{
	SparkFun_APDS9960 apds;
}


__fastcall TfrmCH341I2CApds9960::TfrmCH341I2CApds9960(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CApds9960::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CApds9960::Read(void)
{
	BtnController btnCtrl(btnRead);
	BtnController btnCtrl2(btnInit);

	lblStatus->Caption = "";
	edLight->Text = "";
	edProximity->Text = "";

#if 1
	uint16_t ambient_light = 0;
	uint16_t red_light = 0;
	uint16_t green_light = 0;
	uint16_t blue_light = 0;

	if (  !apds.readAmbientLight(ambient_light) ||
		!apds.readRedLight(red_light) ||
		!apds.readGreenLight(green_light) ||
		!apds.readBlueLight(blue_light) )
	{
		lblStatus->Caption = "Error reading light values";
		return;
	}

	AnsiString text;	
	text.sprintf("ambient %u, red %u, green %u, blue %u", ambient_light, red_light, green_light, blue_light);
	edLight->Text = text;
#endif

#if 1
	uint8_t proximity_data;
	if ( !apds.readProximity(proximity_data) ) {
		lblStatus->Caption = "Error reading proximity value";
	} else {
		edProximity->Text = proximity_data;
	}
#endif

	if ( chbReadGestures->Checked && apds.isGestureAvailable() ) {
		char buf[128];
		struct timeb timebuffer;
		ftime( &timebuffer );
		int size = strftime(buf, sizeof(buf), "%T", localtime(&timebuffer.time));
		snprintf(buf+size, sizeof(buf)-size, ".%03hu ", timebuffer.millitm);
		buf[sizeof(buf)-1] = '\0';
		AnsiString line = buf;
		int val = apds.readGesture();
		switch ( val ) {
		case DIR_UP:
			line += "UP";
			break;
		case DIR_DOWN:
			line += "DOWN";
			break;
		case DIR_LEFT:
			line += "LEFT";
			break;
		case DIR_RIGHT:
			line += "RIGHT";
			break;
		case DIR_NEAR:
			line += "NEAR";
			break;
		case DIR_FAR:
			line += "FAR";
			break;
		default:
			line.cat_printf("NONE (%d)", val);
			break;
		}
		memoGestures->Lines->Add(line);
		SendMessage(memoGestures->Handle, EM_LINESCROLL, 0, memoGestures->Lines->Count);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CApds9960::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CApds9960::btnInitClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	if ( apds.init() == false ) {
		lblStatus->Caption = "APDS-9960 initialization failed";
		return;
	}

	Sleep(50);

	if ( apds.enableLightSensor(false) == false ) {
		lblStatus->Caption = "Error enabling light sensor";
		return;
	}
	// Wait for initialization and calibration to finish
	Sleep(500);

	
	// Adjust the Proximity sensor gain
	if ( !apds.setProximityGain(PGAIN_2X) ) {
		lblStatus->Caption = "Something went wrong trying to set PGAIN";
		return;
	}

	// Start running the APDS-9960 proximity sensor (no interrupts)
	if ( apds.enableProximitySensor(false) == false ) {
		lblStatus->Caption = "Something went wrong while enabling proximity sensor";
		return;
	}

	Sleep(500);

	if (chbReadGestures->Checked) {
		if ( apds.enableGestureSensor(true) == false ) {
			lblStatus->Caption = "Failed to enable gesture sensor";
			return;
		}
		Sleep(500);
	}
}
//---------------------------------------------------------------------------

