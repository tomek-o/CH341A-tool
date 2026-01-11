//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341MdioDetect.h"
#include "CH341A.h"
#include "MDIORegisters.h"
#include "MDIO.h"
#include "TabManager.h"
#include "common/BtnController.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341MdioDetect *frmCH341MdioDetect;
//---------------------------------------------------------------------------
__fastcall TfrmCH341MdioDetect::TfrmCH341MdioDetect(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupGenericInterfaces);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MdioDetect::btnDetectDevicesClick(TObject *Sender)
{
	Detect();
}

void TfrmCH341MdioDetect::Detect(void)
{
	BtnController btnCtrl(btnDetectDevices);
	if (!ch341a.IsOpened())
	{
		memoDetect->Text = "CH341 is not opened!";
		return;
	}

	if (MDIO_Init())
	{
		memoDetect->Text = "Failed to configure CH341A pins!";
		return;
	}

	memoDetect->Clear();
	AnsiString text;
	bool found = false;
	for (unsigned int phyAddress=0; phyAddress<32; phyAddress++)
	{
		AnsiString st;
		st.sprintf("Trying PHY address %u...", phyAddress);
		lblStatus->Caption = st;
		for (unsigned int i=0; i<1; i++)
		{
			uint32_t id1 = MDIO_ReadRegister(phyAddress, MDIO_REG_PHY_ID_1);
			uint32_t id2 = MDIO_ReadRegister(phyAddress, MDIO_REG_PHY_ID_2);
			if (id1 != 0xFFFF || id2 != 0xFFFF)
			{
				text.cat_printf("Found: PHY address %u, ID1 = 0x%04X, ID2 = 0x%04X\r\n",
					phyAddress, id1, id2);
				found = true;
				memoDetect->Text = text;
				break;
			}
			Application->ProcessMessages();
		}
	}
	if (!found)
	{
    	memoDetect->Text = "No MDIO device found (no PHY ID != 0xFFFF on any address)!";
	}
	else
	{
		if (chbBeep->Checked)
		{
			Beep(440, 100);
		}
	}
	lblStatus->Caption = "";	
}
//---------------------------------------------------------------------------

