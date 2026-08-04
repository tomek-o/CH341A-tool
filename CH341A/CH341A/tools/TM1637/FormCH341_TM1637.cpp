//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341_TM1637.h"
#include "TM1637.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341Tm1637 *frmCH341Tm1637;
//---------------------------------------------------------------------------


__fastcall TfrmCH341Tm1637::TfrmCH341Tm1637(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, (1u << ToolGroupDisplays));

	cbBrightness->Clear();
	for (int i=0; i<TM1637::Brightness_Limiter; i++)
	{
    	cbBrightness->Items->Add(TM1637::getBrightnessName(static_cast<TM1637::Brightness>(i)));
	}
	TNotifyEvent event = cbBrightness->OnChange;
	cbBrightness->OnChange = NULL;
	cbBrightness->ItemIndex = TM1637::Brightness_04_16;
	cbBrightness->OnChange = event;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Tm1637::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	tm1637.init();
	int status = tm1637.setBrightness(static_cast<TM1637::Brightness>(cbBrightness->ItemIndex));
	if (status)
	{
		lblStatus->Caption = "No communication with TM1637!";
		return;
	}
	lblStatus->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Tm1637::btnWriteClick(TObject *Sender)
{
	lblStatus->Caption = "";
	unsigned int symbols[TM1637::SYMBOL_COUNT];
	memset(symbols, 0, sizeof(symbols));

    AnsiString text = edText->Text;

	int symbolPos = -1;
	for (int i=1; i<=text.Length(); i++)
	{
		char c = text[i];
		if (c == '.')
		{
			if (symbolPos >= 0)
			{
				symbols[symbolPos] |= TM1637::DECIMAL_POINT;
			}
		}
		else
		{
			symbols[++symbolPos] = c;
		}
	}

	int status = tm1637.writeSymbols(symbols);
	if (status)
	{
    	lblStatus->Caption = "Error writing symbols to TM1637!";
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341Tm1637::cbBrightnessChange(TObject *Sender)
{
	int status = tm1637.setBrightness(static_cast<TM1637::Brightness>(cbBrightness->ItemIndex));
	if (status)
	{
		lblStatus->Caption = "No communication with TM1637!";
		return;
	}
	else
	{
    	lblStatus->Caption = "";
	}
}
//---------------------------------------------------------------------------

