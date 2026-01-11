//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341MdioLan8742Cbln.h"
#include "CH341A.h"
#include "MDIO.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341MdioLan8742Cbln *frmCH341MdioLan8742Cbln;
//---------------------------------------------------------------------------

__fastcall TfrmCH341MdioLan8742Cbln::TfrmCH341MdioLan8742Cbln(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupEthernetMdio);
	for (unsigned int i=0; i<32; i++)
	{
		cbPhyAddress->Items->Add(i);
	}
	cbPhyAddress->ItemIndex = 0;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341MdioLan8742Cbln::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	if (MDIO_Init())
	{
		lblStatus->Caption = "Failed to configure CH341A pins!";
		return;
	}
	lblStatus->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341MdioLan8742Cbln::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341MdioLan8742Cbln::Read(void)
{
	BtnController btnCtrl(btnRead);

	edCbln->Text = "";
	edLength->Text = "";

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint32_t phyAddress = cbPhyAddress->ItemIndex;

	uint16_t val = MDIO_ReadRegister(phyAddress, 28);

	unsigned int cbln = (val & 0xF000) >> 12;
	edCbln->Text = cbln;

	unsigned int length;
	switch (cbln)
	{
	case 4:
		length = 6;
		break;
	case 5:
		length = 17;
		break;
	case 6:
		length = 27;
		break;
	case 7:
		length = 38;
		break;
	case 8:
		length = 49;
		break;
	case 9:
		length = 59;
		break;
	case 10:
		length = 70;
		break;
	case 11:
		length = 81;
		break;
	case 12:
		length = 91;
		break;
	case 13:
		length = 102;
		break;
	case 14:
		length = 113;
		break;
	case 15:
		length = 123;
		break;
	default:
		length = 0;
		break;
	}
	edLength->Text = length;
}
//---------------------------------------------------------------------------





