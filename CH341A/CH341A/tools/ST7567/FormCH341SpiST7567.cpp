//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SpiST7567.h"
#include "ST7567.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "common/TimerController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SpiST7567 *frmCH341SpiST7567;
//---------------------------------------------------------------------------


__fastcall TfrmCH341SpiST7567::TfrmCH341SpiST7567(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiST7567::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	st7567.Init();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiST7567::btnWriteClick(TObject *Sender)
{
	Write();
}

void TfrmCH341SpiST7567::Write(void)
{
	BtnController btnCtrl(btnWrite);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
#if 0
	st7567.Write(0, 0, '0');
	st7567.Write(0, 1, '1');
	st7567.Write(1, 0, '2');
	st7567.Write(1, 1, '3');
	st7567.Write(2, 0, 'A');
#else
	for (int i=0; i<memoText->Lines->Count; i++)
	{
		AnsiString line = memoText->Lines->Strings[i];
		for (int j=1; j<=line.Length(); j++)
		{
			st7567.Write(i, j-1, line[j]);
		}
	}
#endif
}
//---------------------------------------------------------------------------


