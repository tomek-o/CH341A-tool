//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CTSL2591.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CTSL2591 *frmCH341I2CTSL2591;
//---------------------------------------------------------------------------


__fastcall TfrmCH341I2CTSL2591::TfrmCH341I2CTSL2591(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, (1u << ToolGroupSensors));
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CTSL2591::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		Clear();
		return;
	}

	if (tsl2591.begin())
	{
	}
	else
	{
		lblStatus->Caption = "TSL2591 not found!";
		Clear();
		return;
	}

	//displaySensorDetails();


	// You can change the gain on the fly, to adapt to brighter/dimmer light situations
	switch (cbGain->ItemIndex)
	{
	case 0:
		tsl2591.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
		break;
	case 1:
		tsl2591.setGain(TSL2591_GAIN_MED);      // 25x gain
		break;
	case 2:
		tsl2591.setGain(TSL2591_GAIN_HIGH);   // 428x gain
		break;
	default:
		ShowMessage("Unhandled gain!");
		break;
	}
  
	// Changing the integration time gives you a longer time over which to sense light
	// longer timelines are slower, but are good in very low light situtations!
	switch (cbIntegrationTime->ItemIndex)
	{
	case 0:
		tsl2591.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
		break;
	case 1:
		tsl2591.setTiming(TSL2591_INTEGRATIONTIME_200MS);
		break;
	case 2:
		tsl2591.setTiming(TSL2591_INTEGRATIONTIME_300MS);
		break;
	case 3:
		tsl2591.setTiming(TSL2591_INTEGRATIONTIME_400MS);
		break;
	case 4:
		tsl2591.setTiming(TSL2591_INTEGRATIONTIME_500MS);
		break;
	case 5:
		tsl2591.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)
		break;
	default:
		ShowMessage("Unhandled integration time!");
		break;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CTSL2591::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CTSL2591::Read(void)
{
	BtnController btnCtrl(btnRead);


	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		Clear();
		return;
	}

	int status = ch341a.I2CCheckDev(TSL2591_ADDR);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		Clear();
		return;
	}

	if (!tsl2591.isInitialized())
	{
		lblStatus->Caption = "TSL2591 not initialized!";
		Clear();
		return;
	}

	uint32_t lum = tsl2591.getFullLuminosity();
	uint16_t ir, full;
	ir = static_cast<uint16_t>(lum >> 16);
	full = static_cast<uint16_t>(lum & 0xFFFF);
	edChannel0->Text = full;
	edChannel1->Text = ir;
	edVisible->Text = full-ir;
	edLux->Text = FloatToStrF(tsl2591.calculateLux(full, ir), ffFixed, 10, 2);
}

void TfrmCH341I2CTSL2591::Clear(void)
{
	edChannel0->Text = "";
	edChannel1->Text = "";
	edVisible->Text = "";
	edLux->Text = "";
}

//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CTSL2591::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
	{
		Read();
		tmrAutoRead->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CTSL2591::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CTSL2591::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

