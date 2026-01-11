//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

/**
I2C interface/driver for 7-segment LED (4 digits, typically common cathode) and keyboard (multiple connection variants).
Found in WIWA HD-102 DVB-T receiver.
Slightly unusual I2C interface protocol: 1st byte is not a device address, but register number.
*/

#include "FormCH341I2CCH455.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CCH455 *frmCH341I2CCH455;
//---------------------------------------------------------------------------

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))


namespace {

const uint8_t REG_SYSTEM_PARAMETER = 0x48;
const uint8_t REG_DIG0 = 0x68;
const uint8_t REG_DIG1 = 0x6A;
const uint8_t REG_DIG2 = 0x6C;
const uint8_t REG_DIG3 = 0x6E;
const uint8_t REG_KEYCODE = 0x4F;

int WriteReg(uint8_t reg, uint8_t val)
{
	uint8_t writeBuffer[2];
	writeBuffer[0] = reg;
	writeBuffer[1] = val;
	return ch341a.I2CWriteRead(writeBuffer, sizeof(writeBuffer), NULL, 0);
}

}	// namespace

__fastcall TfrmCH341I2CCH455::TfrmCH341I2CCH455(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, (1u << ToolGroupDisplays)|(1u << ToolGroupMisc));
	cbIntensity->ItemIndex = cbIntensity->Items->Count - 1;

	for (unsigned int i=0; i<ARRAY_SIZE(chbBit); i++)
	{
		for (unsigned int j=0; j<ARRAY_SIZE(chbBit[0]); j++)
		{
			TCheckBox *ch = new TCheckBox(this);
			ch->Parent = this;
			ch->Visible = true;
			ch->Top = lblDigit0->Top + (i * (lblDigit1->Top - lblDigit0->Top));
			ch->Left = 6+ lblDigit0->Left + lblDigit0->Width + (j*14);
			ch->OnClick = chbDigitBinClick;
			chbBit[i][j] = ch;
		}
	}

	digitVal[0] = 0x01;
	digitVal[1] = 0x03;
	digitVal[2] = 0x07;
	digitVal[3] = 0x0F;

	AnsiString text;
	text.sprintf("0x%02X", digitVal[0]);
	edDigit0->Text = text;
	text.sprintf("0x%02X", digitVal[1]);
	edDigit1->Text = text;
	text.sprintf("0x%02X", digitVal[2]);
	edDigit2->Text = text;
	text.sprintf("0x%02X", digitVal[3]);
	edDigit3->Text = text;
	UpdateCheckboxes();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CCH455::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	/* bits:   0 INTENS INTENS INTENS 7SEG SLEEP 0 ENA */ 
	uint8_t val = 0;
	val |= static_cast<uint8_t>((cbIntensity->ItemIndex) << 4);
	if (cbMode->ItemIndex == 0)
		val |= 0x08;
	if (chbSleep->Checked)
		val |= 0x04;
	if (chbEnable->Checked)
		val |= 0x01;	

	int status = WriteReg(REG_SYSTEM_PARAMETER, val);
	if (status != 0)
	{
		lblStatus->Caption = "I2C write failed!";
		return;
	}
	else
	{
    	lblStatus->Caption = "";
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CCH455::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CCH455::Read(void)
{
	BtnController btnCtrl(btnRead);


	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

#if 0
	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}
#endif
	uint8_t writeBuffer[1];
	uint8_t readBuffer[1];
	writeBuffer[0] = REG_KEYCODE;
	readBuffer[0] = 0x00;
	int status = ch341a.I2CWriteRead(writeBuffer, sizeof(writeBuffer), readBuffer, sizeof(readBuffer));
	if (status != 0)
	{
		lblStatus->Caption = "I2C write/read error";
		return;
	}

	AnsiString text;
	text.sprintf("Key code: 0x%02X", readBuffer[0]);
	lblStatus->Caption = text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CCH455::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CCH455::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CCH455::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CCH455::btnWriteDigitsClick(TObject *Sender)
{
	BtnController btnCtrl(btnWriteDigits);
#if 0
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
#endif

	int status = 0;
	status |= WriteReg(REG_DIG0, static_cast<uint8_t>(digitVal[0]));
	status |= WriteReg(REG_DIG1, static_cast<uint8_t>(digitVal[1]));
	status |= WriteReg(REG_DIG2, static_cast<uint8_t>(digitVal[2]));
	status |= WriteReg(REG_DIG3, static_cast<uint8_t>(digitVal[3]));

	if (status != 0)
	{
		lblStatus->Caption = "I2C write error!";
		return;
	}
	else
	{
    	lblStatus->Caption = "";
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CCH455::edDigitHexChange(TObject *Sender)
{
    unsigned int digitId;
	if (Sender == edDigit0)
		digitId = 0;
	else if (Sender == edDigit1)
		digitId = 1;
	else if (Sender == edDigit2)
		digitId = 2;
	else if (Sender == edDigit3)
		digitId = 3;
	else
		return;
	TEdit *ed = dynamic_cast<TEdit*>(Sender);
	assert(ed);
	unsigned int val = 0;
	if (sscanf(ed->Text.c_str(), "%X", &val) != 1)
	{
		ed->Color = clRed;
		return;
	}
	ed->Color = clWindow;
	digitVal[digitId] = val;
	UpdateCheckboxes();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CCH455::chbDigitBinClick(TObject *Sender)
{
	for (unsigned int i=0; i<ARRAY_SIZE(chbBit); i++)
	{
		for (unsigned int j=0; j<ARRAY_SIZE(chbBit[0]); j++)
		{
			if (chbBit[i][j] == Sender)
			{
				TCheckBox *chb = chbBit[i][j];
				if (chb->Checked)
					digitVal[i] |= (1 << (7-j));
				else
					digitVal[i] &= ~(1 << (7-j));

				AnsiString text;
				text.sprintf("0x%02X", digitVal[i]);
				switch (i)
				{
				case 0:
					edDigit0->Text = text;
					break;
				case 1:
					edDigit1->Text = text;
					break;
				case 2:
					edDigit2->Text = text;
					break;
				case 3:
					edDigit3->Text = text;
					break;
				default:
					assert(!"Unhandled digit id!");
				}
				break;
			}
		}
	}
}
//---------------------------------------------------------------------------

void TfrmCH341I2CCH455::UpdateCheckboxes(void)
{
	for (unsigned int i=0; i<ARRAY_SIZE(chbBit); i++)
	{
		for (unsigned int j=0; j<ARRAY_SIZE(chbBit[0]); j++)
		{
			TCheckBox *chb = chbBit[i][j];
			chb->OnClick = NULL;
			chb->Checked = digitVal[i] & (1 << (7-j));
			chb->OnClick = chbDigitBinClick;
		}
	}
}

