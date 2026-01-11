//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SHT11.h"
#include "SHT1x.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SHT11 *frmCH341SHT11;
//---------------------------------------------------------------------------


__fastcall TfrmCH341SHT11::TfrmCH341SHT11(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341SHT11::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341SHT11::Read(void)
{
	BtnController btnCtrl(btnRead);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	s_connectionreset();
	int error = 0;
	unsigned char checksum;
	value humid_val,temp_val;
	memset(&humid_val, 0, sizeof(humid_val));
	memset(&temp_val, 0, sizeof(temp_val));
	char szTelnetBuffer[128] = {0};
	char buf[20] = {0};
	//WARNING! s_measure may take up to 2s (max timeout), so you'll probably
	//have to rewrite this function into non-blocking code if your application does sth time-critical
	error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);
	error+=s_measure((unsigned char*) &humid_val.i,&checksum,HUMID);
	if (error != 0)
	{
		s_connectionreset();
		lblStatus->Caption = "MEASUREMENT ERROR!";
	}
	else
	{
		humid_val.f=(float)humid_val.i;
		temp_val.f=(float)temp_val.i;
		float d1;
		switch (cbSupplyVoltage->ItemIndex)
		{
		case 0:
			d1 = 40.1f;
			break;
		case 1:
			d1 = 39.8f;
			break;
		case 2:
			d1 = 39.7f;
			break;
		case 3:
			d1 = 39.6f;
			break;
		case 4:
			d1 = 39.4f;
			break;
		default:
			assert(!"Unhandled supply voltage!");
			return;
		}
		calc_sht11(&humid_val.f,&temp_val.f, d1);
		temp_val.i=(int32_t)(temp_val.f*10);
		humid_val.i = (int32_t)(humid_val.f);

		float dewPoint;
		{
			// calculate dew point temperature
			float Tn, m;
			float temp = static_cast<float>(temp_val.i) / 10.0f;
			float rh = static_cast<float>(humid_val.i);

			if (temp >= 0.0f)
			{
				Tn = 243.12f;
				m = 17.62f;
			}
			else
			{
				Tn = 272.62f;
				m = 22.46f;
			}

			float a = static_cast<float>(log(rh / 100.0f));
			float b = (m * temp) / (Tn + temp);
			dewPoint = Tn * (a + b) / (m - a - b);
		}

		strcpy(szTelnetBuffer, "Temperature: ");
		if (temp_val.i < 0)
			strcat(szTelnetBuffer, "-");
		temp_val.i = abs(temp_val.i);
		itoa(temp_val.i/10, buf, 10);
		strcat(szTelnetBuffer, buf);
		strcat(szTelnetBuffer,".");
		itoa(temp_val.i%10, buf, 10);
		strcat(szTelnetBuffer,buf);
		strcat(szTelnetBuffer,"C     ");

		strcat(szTelnetBuffer,"RH: ");
		itoa(humid_val.i, buf, 10);
		strcat(szTelnetBuffer,buf);
		strcat(szTelnetBuffer,"%");

		AnsiString text = szTelnetBuffer;
		text.cat_printf("\r\nDew point: %.1fC", dewPoint);
		lblStatus->Caption = text;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SHT11::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SHT11::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SHT11::chbAutoReadKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

