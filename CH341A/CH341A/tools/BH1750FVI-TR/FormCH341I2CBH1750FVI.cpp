//---------------------------------------------------------------------------
//by SimoSbara

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CBH1750FVI.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CBH1750FVI *frmCH341I2CBH1750FVI;
//---------------------------------------------------------------------------


__fastcall TfrmCH341I2CBH1750FVI::TfrmCH341I2CBH1750FVI(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, (1u << ToolGroupSensors));
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBH1750FVI::btnOpenClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		Clear();
		return;
	}

	if (!bh1750fvi.open())
	{
		lblStatus->Caption = "BH1750FVI not found!";
		Clear();
		return;
	}

	lblStatus->Caption = "BH1750FVI opened!";
	btnClose->Enabled = true;
	btnRead->Enabled = true;
	cbMode->Enabled = true;
}

void __fastcall TfrmCH341I2CBH1750FVI::btnCloseClick(TObject *Sender)
{
	if (!bh1750fvi.close())
	{
		lblStatus->Caption = "failed to close BH1750FVI!";
		return;
	}

	lblStatus->Caption = "BH1750FVI closed!";
	btnClose->Enabled = false;
	btnRead->Enabled = false;
	cbMode->Enabled = false;
	Clear();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBH1750FVI::btnReadClick(TObject *Sender)
{
	if(!chbAutoRead->Checked)
		Read(true);
}

void TfrmCH341I2CBH1750FVI::Read(bool single)
{
	if (!bh1750fvi.isInitialized())
	{
		lblStatus->Caption = "BH1750FVI not initialized!";
		Clear();
		return;
	}

	uint8_t mode = 0;
	int16_t ms = 0;

	switch(cbMode->ItemIndex)
	{
		case 0:
			mode = BH1750FVI_HRES1;
			ms = 140;
			break;
		case 1:
			mode = BH1750FVI_HRES2;
			ms = 140;
			break;
		case 2:
			mode = BH1750FVI_LRES;
            ms = 16;	
			break;
		default:
		{
			lblStatus->Caption = "Invalid mode!";
			Clear();
			return;
        }
	}

	if(!bh1750fvi.start(mode, false))
	{
		lblStatus->Caption = "Failed to start read!";
		Clear();
		return;
	}

	//wait for values
	do
	{
		Sleep(20);
		Application->ProcessMessages();
		ms -= 20;
	} while (ms > 0);

	uint16_t l = bh1750fvi.getLuminosity();
	float lux = bh1750fvi.calculateLux(l);

	edRawData->Text = l;
	edLux->Text = FloatToStrF(lux, ffFixed, 10, 2);
}

void TfrmCH341I2CBH1750FVI::Clear(void)
{
	edLux->Text = "";
	edRawData->Text = "";
}

//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CBH1750FVI::tmrAutoReadTimer(TObject *Sender)
{
	if(!bh1750fvi.isInitialized())
    	return;

	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
	{
		Read(false);
		tmrAutoRead->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBH1750FVI::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBH1750FVI::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------




