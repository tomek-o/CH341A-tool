//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341MdioWriteRead.h"
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
TfrmCH341MdioWriteRead *frmCH341MdioWriteRead;
//---------------------------------------------------------------------------


__fastcall TfrmCH341MdioWriteRead::TfrmCH341MdioWriteRead(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupGenericInterfaces);
	for (unsigned int i=0; i<32; i++)
	{
		cbPhyAddress->Items->Add(i);
	}
	cbPhyAddress->ItemIndex = 0;

	for (unsigned int i=0; i<32; i++)
	{
		AnsiString caption;
		AnsiString description;

		const struct MdioRegDescription* rd = FindRegDescription(i);
		if (rd)
		{
			description = rd->caption;
		}
		caption.printf("%2u %s", i, description.c_str());
		cbRegId->Items->Add(caption);
	}

	if (static_cast<unsigned int>(cbRegId->Items->Count) > 0)
	{
		cbRegId->ItemIndex = 0;
		cbRegIdChange(NULL);
	}
	chlbox->DoubleBuffered = true;
	edValueHex->DoubleBuffered = true;
	edValueDec->DoubleBuffered = true;
}
//---------------------------------------------------------------------------

const struct MdioRegDescription* TfrmCH341MdioWriteRead::FindRegDescription(int id)
{
	const struct MdioRegDescription* regs;
	unsigned int regsCount = 0;
	MdioGetRegisterDescriptions(regs, regsCount);
	for (unsigned int r=0; r<regsCount; r++)
	{
		if (regs[r].reg == id)
		{
        	return &regs[r];
		}
	}
	return NULL;
}

void __fastcall TfrmCH341MdioWriteRead::btnInitClick(TObject *Sender)
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

void __fastcall TfrmCH341MdioWriteRead::btnReadRegisterClick(TObject *Sender)
{
	BtnController btnCtrl(btnReadRegister);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";

	uint32_t phyAddress = cbPhyAddress->ItemIndex;
	uint32_t phyRegister = cbRegId->ItemIndex;

	uint16_t val = MDIO_ReadRegister(phyAddress, phyRegister);

	UpdateValueDisplay(val, NULL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341MdioWriteRead::cbRegIdChange(TObject *Sender)
{
	const struct MdioRegDescription* rd = FindRegDescription(cbRegId->ItemIndex);
	if (rd)
	{
		btnReadRegister->Enabled = rd->rFlag;
		btnWriteRegister->Enabled = rd->wFlag;
	}
	else
	{
		btnReadRegister->Enabled = true;
		btnWriteRegister->Enabled = true;
	}

	for (unsigned int i=0; i<16; i++)
	{
		AnsiString text;
		text.sprintf("bit %02d    %s", 15-i, rd ? rd->bitCaption[15-i].c_str() : "");
		chlbox->Items->Strings[i] = text;
	}

	UpdateValueDisplay(0, NULL);	
}
//---------------------------------------------------------------------------


void TfrmCH341MdioWriteRead::UpdateValueDisplay(uint16_t value, TObject *Sender)
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

void __fastcall TfrmCH341MdioWriteRead::edValueHexChange(TObject *Sender)
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

void __fastcall TfrmCH341MdioWriteRead::edValueDecChange(TObject *Sender)
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

void __fastcall TfrmCH341MdioWriteRead::chlboxClickCheck(TObject *Sender)
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

void __fastcall TfrmCH341MdioWriteRead::btnWriteRegisterClick(TObject *Sender)
{
	BtnController btnCtrl(btnWriteRegister);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	lblStatus->Caption = "";

	uint32_t phyAddress = cbPhyAddress->ItemIndex;
	uint32_t phyRegister = cbRegId->ItemIndex;

	uint16_t value = 0;
	for (unsigned int i=0; i<16; i++)
	{
		if (chlbox->Checked[i])
		{
			value |= static_cast<uint16_t>(1 << (15-i));
		}
	}

	MDIO_WriteRegister(phyAddress, phyRegister, value);
}
//---------------------------------------------------------------------------

