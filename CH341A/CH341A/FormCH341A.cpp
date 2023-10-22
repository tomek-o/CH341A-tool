//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341A.h"
#include "CH341A.h"
#include "Settings.h"
#include "TabManager.h"
#include "Log.h"
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfrmCH341A::TfrmCH341A(TComponent* Owner)
	: TForm(Owner),
	devIndex(0)
{
	TabManager::Instance().Configure(pages, tvTools);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341A::btnOpenClick(TObject *Sender)
{
	int status = ch341a.Open(devIndex, appSettings.ch341a);
	if (status != 0)
	{
		MessageBox(this->Handle, "Could not open CH341A device", this->Caption.c_str(), MB_ICONEXCLAMATION);
	}
	else
	{
		btnOpen->Enabled = false;
		btnClose->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341A::btnCloseClick(TObject *Sender)
{
	ch341a.Close();
	btnOpen->Enabled = true;
	btnClose->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341A::tvToolsChange(TObject *Sender, TTreeNode *Node)
{
	TabManager::Instance().SwitchToPage(Node->Index);
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341A::tmrStartupTimer(TObject *Sender)
{
	tmrStartup->Enabled = false;

	if (tvTools->Items->Count)
	{
		tvTools->Items->Item[0]->Selected = true;
		tvToolsChange(NULL, tvTools->Items->Item[0]);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341A::FormShow(TObject *Sender)
{
	if (appSettings.ch341a.openAtStartup)
	{
		int status = ch341a.Open(devIndex, appSettings.ch341a);
		if (status != 0)
		{
			LOG("Could not open CH341A device at startup", this->Caption.c_str(), MB_ICONEXCLAMATION);
		}
		else
		{
			btnOpen->Enabled = false;
			btnClose->Enabled = true;
		}
	}
}
//---------------------------------------------------------------------------

