//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CTea5767.h"
#include "CH341A.h"
#include "TEA5767.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CTea5767 *frmCH341I2CTea5767;
//---------------------------------------------------------------------------

__fastcall TfrmCH341I2CTea5767::TfrmCH341I2CTea5767(TComponent* Owner)
	: TForm(Owner),
	searching(false),
	searchingUp(true)
{
	TabManager::Instance().Register(this);
	btnInit->Enabled = true;
	btnStopHandling->Enabled = false;
}
//---------------------------------------------------------------------------
void TfrmCH341I2CTea5767::Read(void)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";

	struct TEA5767_status status;
	memset(&status, 0, sizeof(status));	

	TEA5767_get_status(&status);

	AnsiString text, text2;

	text.sprintf("STATUS: %.02f MHz%s",
		static_cast<float>(status.frequency) / 1000.0f,
    	searching ? ", searching" : ""
	);

	text2.sprintf("%s, %s, %s, RX power = %u",
		status.ready ? "READY" : "NOT READY",
		status.tuned ? "TUNED" : "NOT TUNED",
		status.stereo ? "STEREO" : "MONO",
		status.rx_power
	);

	LOG("TEA5767: %s, %s\n", text.c_str(), text2.c_str());

	if (status.ready && searching)
	{
		if (status.band_limit)
		{
			LOG("TEA5767: band limit on search, wrapping around...\n");
			if (searchingUp == false)
			{
				TEA5767_tune(108000UL);
			}
			else
			{
				TEA5767_tune(87500UL);
			}
			TEA5767_search(searchingUp);
		}
		else
		{
			LOG("TEA5767: end of search\n");
			searching = false;
			TEA5767_exit_search();
		}
	}

	lblTeaStatus->Caption = text;
	lblTeaStatus2->Caption = text2;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CTea5767::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CTea5767::btnInitClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";

	TEA5767_init();
	TEA5767_tune(99900);	
	TEA5767_write();
	searching = false;	
	Sleep(500);
	Read();
	tmrAutoRead->Enabled = true;
	btnInit->Enabled = false;
	btnStopHandling->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CTea5767::btnSearchUpClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";

	searching = true;
	searchingUp = true;
	TEA5767_search(searchingUp);
	Sleep(100);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CTea5767::btnSearchDownClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";

	searching = true;
	searchingUp = false;
	TEA5767_search(searchingUp);
	Sleep(100);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CTea5767::btnTuneByClick(TObject *Sender)
{
	TButton *button = dynamic_cast<TButton*>(Sender);
	assert(button);
	uint32_t tune = TEA5767_get_tune();
	tune += button->Tag;
	TEA5767_tune(tune);
	TEA5767_write();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CTea5767::btnStopHandlingClick(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	btnInit->Enabled = true;
	btnStopHandling->Enabled = false;
}
//---------------------------------------------------------------------------

