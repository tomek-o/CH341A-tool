//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CMax11601.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CMax11601 *frmCH341I2CMax11601;
//---------------------------------------------------------------------------

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

namespace
{

enum { SLAVE_ADDR = 0x64 };

const double voltageVdd = 3.3;
const double voltageInternalRef = 2.048;

double GetVoltage(uint8_t val, double voltageReference) {
	return voltageReference / 256 * val;
}

struct ValueDescription {
	uint8_t value;
	const char* description;
};

/* 	The master can write either 1 or 2 bytes to the slave in any order (setup byte then
	configuration byte; configuration byte then setup byte; setup byte only; configuration byte only.
*/

const unsigned int REF_SEL_OFFSET = 4;
const unsigned int REF_SEL_MASK = 0x07 << REF_SEL_OFFSET;
const uint8_t REF_VDD = 0x00;
const uint8_t REF_EXT = 0x02;
const uint8_t REF_INT_ADC3_IN = 0x05;
const uint8_t REF_INT_ADC3_OUT = 0x06;
ValueDescription refSel[] = {
	{ REF_VDD, "Vdd (assuming 3.3V for calculation)" },
	{ REF_EXT, "external (REF pin as reference input)" },
	{ REF_INT_ADC3_IN, "internal (2.048V) with REF pin as analog input" },
	{ REF_INT_ADC3_OUT, "internal (2.048V) with REF pin as reference output" },
};

}	// namespace

__fastcall TfrmCH341I2CMax11601::TfrmCH341I2CMax11601(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupAdc);
	cbReferenceVoltage->Clear();
	for (unsigned int i=0; i<ARRAY_SIZE(refSel); i++)
	{
		cbReferenceVoltage->Items->Add(refSel[i].description);
	}
	cbReferenceVoltage->ItemIndex = 0;
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341I2CMax11601::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CMax11601::Read(void)
{
	BtnController btnCtrl(btnRead);
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	/** \note 	This ADC applies clock stretching. It is (most likely) not supported
				by CH341, thus I2C speed has to be reduced.
	*/
	if (ch341a.GetI2CSpeed() != CH341AConf::I2C_SPEED_20K)
	{
		lblStatus->Caption = "CH341 I2C speed must be configured as 20 kHz!";
		return;
	}

	uint8_t address = SLAVE_ADDR;

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	lblStatus->Caption = "";

	uint8_t buffer[2];	// setup + config
	// BIT 7 BIT 6 BIT 5 BIT 4 BIT 3 BIT 2 BIT 1 BIT 0
	// REG   SEL2  SEL1  SEL0  CLK   BIPOL nRST  unused
	buffer[0] = 0xD2;	// setup: REG=1, internal vref, internal clock, unipolar
	buffer[0] &= ~REF_SEL_MASK;
	buffer[0] |= static_cast<uint8_t>(refSel[cbReferenceVoltage->ItemIndex].value << REF_SEL_OFFSET);
	// BIT 7 BIT 6 BIT 5 BIT 4 BIT 3 BIT 2 BIT 1 BIT 0
	// REG   SCAN1 SCAN0 CS3   CS2   CS1   CS0   SGL/nDIF
	buffer[1] = 0x07;	// configuration: scan inputs from 0 to 3, non-differential
	//LOG("MAX11601: write setup 0x%02X, config 0x%02X\n", static_cast<unsigned int>(buffer[0]),static_cast<unsigned int>(buffer[1]));  

	status = ch341a.I2CWriteBytes(address, buffer, sizeof(buffer));
	if (status != 0)
	{
		lblStatus->Caption = "I2C write failed!";
		return;
	}

	enum { INPUT_COUNT = 4 };
	uint8_t rawValues[INPUT_COUNT];
	memset(rawValues, 0, sizeof(rawValues));
	status = ch341a.I2CReadBytes(address, rawValues, sizeof(rawValues));
	if (status != 0)
	{
		lblStatus->Caption = "I2C read failed!";
		return;
	}

	AnsiString text[INPUT_COUNT];

	double referenceVoltage = -1;
	unsigned int refType = refSel[cbReferenceVoltage->ItemIndex].value;
	switch (refType)
	{
	case REF_VDD:
		referenceVoltage = voltageVdd;
		break;
	case REF_EXT:
		referenceVoltage = -1;
		break;
	case REF_INT_ADC3_IN:
		referenceVoltage = voltageInternalRef;
		break;
	case REF_INT_ADC3_OUT:
		referenceVoltage = voltageInternalRef;
		break;
	default:
		break;
	}

	for (unsigned int i=0; i<INPUT_COUNT; i++)
	{
		text[i].sprintf("RAW: %3u", rawValues[i]);
		if (referenceVoltage > 0)
		{
        	text[i].cat_printf("    =>    %.3f V", GetVoltage(rawValues[i], referenceVoltage));
		}
	}
	edAin0->Text = text[0];
	edAin1->Text = text[1];
	edAin2->Text = text[2];
	edAin3->Text = text[3];
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CMax11601::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMax11601::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMax11601::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CMax11601::cbReferenceVoltageChange(TObject *Sender)
{
	unsigned int refType = refSel[cbReferenceVoltage->ItemIndex].value;
	lblAin3->Visible = (refType != REF_INT_ADC3_OUT);
	edAin3->Visible = (refType != REF_INT_ADC3_OUT);
}
//---------------------------------------------------------------------------

