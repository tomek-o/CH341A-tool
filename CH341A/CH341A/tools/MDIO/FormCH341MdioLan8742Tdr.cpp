//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341MdioLan8742Tdr.h"
#include "MDIO.h"
#include "MDIORegisters.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341MdioLan8742Tdr *frmCH341MdioLan8742Tdr;
//---------------------------------------------------------------------------

__fastcall TfrmCH341MdioLan8742Tdr::TfrmCH341MdioLan8742Tdr(TComponent* Owner)
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

void __fastcall TfrmCH341MdioLan8742Tdr::btnInitClick(TObject *Sender)
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

void __fastcall TfrmCH341MdioLan8742Tdr::btnTestClick(TObject *Sender)
{
	Read();
}

void TfrmCH341MdioLan8742Tdr::Read(void)
{
	BtnController btnCtrl(btnTest);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint32_t phyAddress = cbPhyAddress->ItemIndex;

	// disable ANEG, force 100 Mbps FD
	MDIO_WriteRegister(phyAddress, 0, 0x2100);

	if (cbPair->ItemIndex == 0 || cbPair->ItemIndex == 1)
	{
		TestPair(phyAddress, true, edCableTxState, edCableTxLengthReg, edCableTxLengthMeters);
	}

	if (cbPair->ItemIndex == 0 || cbPair->ItemIndex == 2)
	{
		TestPair(phyAddress, false, edCableRxState, edCableRxLengthReg, edCableRxLengthMeters);
	}
}




void TfrmCH341MdioLan8742Tdr::TestPair(unsigned int phyAddress, bool txPair, TEdit *edState, TEdit *edLengthReg, TEdit *edLengthMeters)
{
	edState->Text = "";
	edLengthReg->Text = "";
	edLengthMeters->Text = "";
	
	// disable AMDIX and force MDI (or MDIX)
	// TX pair = MDI, RX pair = MDIX
	Result *res;
	if (txPair)
	{
		MDIO_WriteRegister(phyAddress, 27, 0x8000);
		res = &resultTx;
	}
	else
	{
		MDIO_WriteRegister(phyAddress, 27, 0xA000);
		res = &resultRx;
	}

	// enable TDR
	MDIO_WriteRegister(phyAddress, 25, 0x8000);

	// check TDR control/status register
	bool done = false;
	uint16_t val;
	for (unsigned int i=0; i<3; i++)
	{
		val = MDIO_ReadRegister(phyAddress, 25);
		if ((val & 0x8000) == 0)
		{
			done = true;
			break;
		}
	}
	if (!done)
	{
		lblStatus->Caption = "Timed out waiting for TDR result!";
		return;
	}
	res->cableType = (val & 0x0600) >> 9;
	res->cableLengthReg = val & 0x00FF;

	switch (res->cableType)
	{
	case 1:
		edState->Text = "shorted";
		break;
	case 2:
		edState->Text = "open";
		break;
	case 3:
		edState->Text = "matched";
		break;
	default:
		edState->Text = "";
		break;
	}

	edLengthReg->Text = res->cableLengthReg;
    UpdateLength();
}
//---------------------------------------------------------------------------

void TfrmCH341MdioLan8742Tdr::UpdateLength(void)
{
	float propagationConst;

	propagationConst = GetPropagationConst(resultTx);
	if (propagationConst > 0)
	{
		edCableTxLengthMeters->Text = FloatToStrF(propagationConst * resultTx.cableLengthReg, ffFixed, 5, 3);
	}
	else
	{
		edCableTxLengthMeters->Text = "n/a"; 
	}

	propagationConst = GetPropagationConst(resultRx);
	if (propagationConst > 0)
	{
		edCableRxLengthMeters->Text = FloatToStrF(propagationConst * resultRx.cableLengthReg, ffFixed, 5, 3);
	}
	else
	{
		edCableRxLengthMeters->Text = "n/a"; 
	}
}

float TfrmCH341MdioLan8742Tdr::GetPropagationConst(const Result &res)
{
	if (res.cableType == 1)	// shorted
	{
		switch (cbCableType->ItemIndex)
		{
		case 1:
			return 0.759f;
		case 2:
			return 0.788f;
		case 3:
			return 0.873f;
		default:
			return 0.793f;
		}
	}
	else if (res.cableType == 2)	// open
	{
		switch (cbCableType->ItemIndex)
		{
		case 1:
			return 0.745f;
		case 2:
			return 0.76f;
		case 3:
			return 0.85f;
		default:
			return 0.769f;
		}
	}
	else
	{
    	return -1;
	}
}

void __fastcall TfrmCH341MdioLan8742Tdr::cbCableTypeChange(TObject *Sender)
{
	UpdateLength();	
}
//---------------------------------------------------------------------------

