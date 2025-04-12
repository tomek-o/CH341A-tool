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
	this->DoubleBuffered = true;
	pages->DoubleBuffered = true;
	pnlTop->DoubleBuffered = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341A::btnOpenClick(TObject *Sender)
{
	DeviceOpen();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341A::btnCloseClick(TObject *Sender)
{
	DeviceClose();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341A::tvToolsChange(TObject *Sender, TTreeNode *Node)
{
	TabManager::Instance().SwitchToPage(Node->Index);
	appSettings.ch341a.lastPage = Node->Text;
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341A::tmrStartupTimer(TObject *Sender)
{
	tmrStartup->Enabled = false;

	if (tvTools->Items->Count)
	{
		int itemId = 0;
		if (appSettings.ch341a.lastPage != "")
		{
			for (int i=0; i<tvTools->Items->Count; i++)
			{
				if (tvTools->Items->Item[i]->Text == appSettings.ch341a.lastPage)
				{
					itemId = i;
					break;
				}
			}
		}
		tvTools->Items->Item[itemId]->Selected = true;
		tvToolsChange(NULL, tvTools->Items->Item[itemId]);
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

int TfrmCH341A::DeviceOpen(void)
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
	return status;
}

void TfrmCH341A::DeviceClose(void)
{
	ch341a.Close();
	btnOpen->Enabled = true;
	btnClose->Enabled = false;
}

void TfrmCH341A::DeviceReopen(void)
{
	DeviceClose();
	DeviceOpen();
}

