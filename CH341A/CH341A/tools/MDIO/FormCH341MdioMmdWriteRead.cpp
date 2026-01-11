//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341MdioMmdWriteRead.h"
#include "MDIO.h"
#include "MDIORegisters.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341MdioMmdWriteRead *frmCH341MdioMmdWriteRead;
//---------------------------------------------------------------------------


__fastcall TfrmCH341MdioMmdWriteRead::TfrmCH341MdioMmdWriteRead(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupGenericInterfaces);
	for (unsigned int i=0; i<32; i++)
	{
		cbPhyAddress->Items->Add(i);
	}
	cbPhyAddress->ItemIndex = 0;

	chlbox->DoubleBuffered = true;
	edValueHex->DoubleBuffered = true;
	edValueDec->DoubleBuffered = true;

	for (unsigned int i=0; i<16; i++)
	{
		AnsiString text;
		text.sprintf("bit %02d    %s", 15-i, "");
		chlbox->Items->Strings[i] = text;
	}

	UpdateValueDisplay(0, NULL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341MdioMmdWriteRead::btnInitClick(TObject *Sender)
{
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

void __fastcall TfrmCH341MdioMmdWriteRead::btnReadRegisterClick(TObject *Sender)
{
	BtnController btnCtrl(btnReadRegister);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";

	uint32_t phyAddress = cbPhyAddress->ItemIndex;
	int device = StrToIntDef(edDevice->Text, -1);
	if (device < 0 || device >= 32768)
	{
		MessageBox(this->Handle, "Invalid device address!", this->Caption.c_str(), MB_ICONEXCLAMATION);
    	return;
	}
	int reg = StrToIntDef(edRegister->Text, -1);
	if (reg < 0 || reg >= 32768)
	{
		MessageBox(this->Handle, "Invalid register index!", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	uint16_t val = MDIO_ReadRegisterClause22for45(phyAddress, static_cast<uint16_t>(device), reg);

	UpdateValueDisplay(val, NULL);
}
//---------------------------------------------------------------------------



void TfrmCH341MdioMmdWriteRead::UpdateValueDisplay(uint16_t value, TObject *Sender)
{
	AnsiString text;

	edValueHex->OnChange = NULL;
	edValueDec->OnChange = NULL;
	chlbox->OnClickCheck = NULL;

	if (Sender != edValueHex)
	{
		text.sprintf("0x%04X", value);
		edValueHex->Text = text;
	}

	if (Sender != edValueDec)
	{
		text.sprintf("%u", value);
		edValueDec->Text = text;
	}

	if (Sender != chlbox)
	{
		for (unsigned int i=0; i<16; i++)
		{
			chlbox->Checked[i] = value & (1 << (15-i));
		}
	}

	edValueHex->OnChange = edValueHexChange;
	edValueDec->OnChange = edValueDecChange;
	chlbox->OnClickCheck = chlboxClickCheck;
}

void __fastcall TfrmCH341MdioMmdWriteRead::edValueHexChange(TObject *Sender)
{
	unsigned int value;
	if (sscanf(edValueHex->Text.c_str(), "%X", &value) == 1)
	{
		if (value <= 65535)
		{
			UpdateValueDisplay(static_cast<uint16_t>(value), edValueHex);
		}
	}	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341MdioMmdWriteRead::edValueDecChange(TObject *Sender)
{
	unsigned int value;
	if (sscanf(edValueDec->Text.c_str(), "%u", &value) == 1)
	{
		if (value <= 65535)
		{
        	UpdateValueDisplay(static_cast<uint16_t>(value), edValueDec);
		}
	}	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341MdioMmdWriteRead::chlboxClickCheck(TObject *Sender)
{
	uint16_t value = 0;
	for (unsigned int i=0; i<16; i++)
	{
		if (chlbox->Checked[i])
		{
			value |= static_cast<uint16_t>(1 << (15-i));
		}
	}
	UpdateValueDisplay(value, chlbox);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341MdioMmdWriteRead::btnWriteRegisterClick(TObject *Sender)
{
	BtnController btnCtrl(btnWriteRegister);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";

	uint32_t phyAddress = cbPhyAddress->ItemIndex;
	int device = StrToIntDef(edDevice->Text, -1);
	if (device < 0 || device >= 32768)
	{
		MessageBox(this->Handle, "Invalid device address!", this->Caption.c_str(), MB_ICONEXCLAMATION);
    	return;
	}
	int reg = StrToIntDef(edRegister->Text, -1);
	if (reg < 0 || reg >= 32768)
	{
		MessageBox(this->Handle, "Invalid register index!", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	uint16_t value = 0;
	for (unsigned int i=0; i<16; i++)
	{
		if (chlbox->Checked[i])
		{
			value |= static_cast<uint16_t>(1 << (15-i));
		}
	}
	MDIO_WriteRegisterClause22for45(phyAddress, static_cast<uint16_t>(device), reg, value);
}
//---------------------------------------------------------------------------

