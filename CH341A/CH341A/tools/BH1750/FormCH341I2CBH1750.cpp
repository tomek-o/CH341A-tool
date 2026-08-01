//---------------------------------------------------------------------------
/*
MIT License

Copyright (c) 2026 SimoSbara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CBH1750.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CBH1750 *frmCH341I2CBH1750;
//---------------------------------------------------------------------------


__fastcall TfrmCH341I2CBH1750::TfrmCH341I2CBH1750(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, (1u << ToolGroupSensors));
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBH1750::btnOpenClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		Clear();
		return;
	}

	uint8_t address = (cbAddress->ItemIndex == 0) ? BH1750::ADDR_A : BH1750::ADDR_B;

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	if (!bh1750.open(address))
	{
		lblStatus->Caption = "BH1750 opening failed!";
		Clear();
		return;
	}

	lblStatus->Caption = "BH1750 opened!";
	btnClose->Enabled = true;
	btnRead->Enabled = true;
	cbMode->Enabled = true;
}

void __fastcall TfrmCH341I2CBH1750::btnCloseClick(TObject *Sender)
{
	if (!bh1750.close())
	{
		lblStatus->Caption = "failed to close BH1750!";
		return;
	}

	lblStatus->Caption = "BH1750 closed!";
	btnClose->Enabled = false;
	btnRead->Enabled = false;
	cbMode->Enabled = false;
	Clear();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBH1750::btnReadClick(TObject *Sender)
{
	if(!chbAutoRead->Checked)
		Read(true);
}

void TfrmCH341I2CBH1750::Read(bool single)
{
	if (!bh1750.isInitialized())
	{
		lblStatus->Caption = "BH1750 not initialized!";
		Clear();
		return;
	}

	uint8_t mode;
	int ms;

	switch(cbMode->ItemIndex)
	{
		case 0:
			mode = BH1750::MODE_HRES1;
			ms = 140;
			break;
		case 1:
			mode = BH1750::MODE_HRES2;
			ms = 140;
			break;
		case 2:
			mode = BH1750::MODE_LRES;
            ms = 16;	
			break;
		default:
		{
			lblStatus->Caption = "Invalid mode!";
			Clear();
			return;
        }
	}

	if(!bh1750.start(mode, false))
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

	uint16_t l = bh1750.getLuminosity();
	float lux = bh1750.calculateLux(l);

	edRawData->Text = l;
	edLux->Text = FloatToStrF(lux, ffFixed, 10, 2);
}

void TfrmCH341I2CBH1750::Clear(void)
{
	edLux->Text = "";
	edRawData->Text = "";
}

//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CBH1750::tmrAutoReadTimer(TObject *Sender)
{
	if(!bh1750.isInitialized())
    	return;

	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
	{
		Read(false);
		tmrAutoRead->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBH1750::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CBH1750::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------




