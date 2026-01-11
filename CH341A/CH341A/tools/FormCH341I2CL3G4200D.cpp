//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CL3G4200D.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CL3G4200D *frmCH341I2CL3G4200D;
//---------------------------------------------------------------------------

#define L3G4200D_ADDRESS_A			0x68
#define L3G4200D_ADDRESS_B			0x69

#define L3G4200D_WHO_AM_I			0x0F
#define L3G4200D_CTRL_REG1			0x20
#define L3G4200D_CTRL_REG2			0x21
#define L3G4200D_CTRL_REG3			0x22
#define L3G4200D_CTRL_REG4			0x23
#define L3G4200D_CTRL_REG5			0x24
#define L3G4200D_REFERENCE			0x25
#define L3G4200D_OUT_TEMP			0x26
#define L3G4200D_STATUS_REG			0x27
#define L3G4200D_OUT_X_L			0xA8
#define L3G4200D_OUT_X_H			0x29
#define L3G4200D_OUT_Y_L			0x2A
#define L3G4200D_OUT_Y_H			0x2B
#define L3G4200D_OUT_Z_L			0x2C
#define L3G4200D_OUT_Z_H			0x2D
#define L3G4200D_FIFO_CTRL_REG		0x2E
#define L3G4200D_FIFO_SRC_REG		0x2F
#define L3G4200D_INT1_CFG			0x30
#define L3G4200D_INT1_SRC			0x31
#define L3G4200D_INT1_TSH_XH		0x32
#define L3G4200D_INT1_TSH_XL		0x33
#define L3G4200D_INT1_TSH_YH		0x34
#define L3G4200D_INT1_TSH_YL		0x35
#define L3G4200D_INT1_TSH_ZH		0x36
#define L3G4200D_INT1_TSH_ZL		0x37
#define L3G4200D_INT1_DURATION		0x38

enum L3G4200D_SCALE {
	L3G4200D_SCALE_250DPS  = 0x00,
	L3G4200D_SCALE_500DPS  = 0x01,
	L3G4200D_SCALE_2000DPS = 0x02,
};


__fastcall TfrmCH341I2CL3G4200D::TfrmCH341I2CL3G4200D(TComponent* Owner)
	: TForm(Owner),
	dpsPerBit(0)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);
}
//---------------------------------------------------------------------------

uint8_t TfrmCH341I2CL3G4200D::GetAddress(void)
{
	switch (cbAddress->ItemIndex)
	{
	case 0:
		return L3G4200D_ADDRESS_A;
	case 1:
		return L3G4200D_ADDRESS_B;
	default:
		assert(!"Unhandled address!");
		return 0;
	}
}

void __fastcall TfrmCH341I2CL3G4200D::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();

	uint8_t data = 0xFF;
	int status = ch341a.I2CWriteCommandReadByte(address, L3G4200D_WHO_AM_I, data);
	if (status != 0 || data != 0xD3)
	{
		lblStatus->Caption = "Failed to identify ID (WHO_AM_I)";
		return;
	}

	uint8_t reg1 = 0x00;
	reg1 |= 0x0F; 			// Enable all axis and setup normal mode
	reg1 |= (0x00 << 4);	// Set output data rate (lowest) + bandwidh
	status = ch341a.I2CWriteCommandWriteByte(address, L3G4200D_CTRL_REG1, reg1);
	if (status != 0)
	{
		lblStatus->Caption = "Failed to write to REG1";
		return;
	}

    // Disable high pass filter
	status = ch341a.I2CWriteCommandWriteByte(address, L3G4200D_CTRL_REG2, 0x00);
	if (status != 0)
	{
		lblStatus->Caption = "Failed to write to REG2";
		return;
	}

	enum L3G4200D_SCALE scale = L3G4200D_SCALE_2000DPS;
	(void)scale;
	switch (cbRange->ItemIndex)
	{
		case 0:
			scale = L3G4200D_SCALE_250DPS;
			dpsPerBit = 0.00875;
			break;
		case 1:
			scale = L3G4200D_SCALE_500DPS;
			dpsPerBit = 0.0175;
			break;
		default:
			scale = L3G4200D_SCALE_2000DPS;
			dpsPerBit = 0.07;
			break;
	}

	// Set full scale selection in continous mode
#ifdef __BORLANDC__
#pragma warn -8071
#endif
	status = ch341a.I2CWriteCommandWriteByte(address, L3G4200D_CTRL_REG4, scale << 4);
#ifdef __BORLANDC__
#pragma warn .8071
#endif
	if (status != 0)
	{
		lblStatus->Caption = "Failed to write to REG4";
		return;
	}

	// normal mode, disabled FIFO, HPF disabled
	status = ch341a.I2CWriteCommandWriteByte(address, L3G4200D_CTRL_REG5, 0x00);
	if (status != 0)
	{
		lblStatus->Caption = "Failed to write to REG5";
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CL3G4200D::btnReadClick(TObject *Sender)
{
	Read();
}

AnsiString TfrmCH341I2CL3G4200D::RawValToString(int val)
{
	AnsiString text = FloatToStrF(dpsPerBit * val, ffFixed, 6, 1);
	if (val >= 0)
		text = (AnsiString)" " + text;
	return text;
}

void TfrmCH341I2CL3G4200D::Read(void)
{
	BtnController btnCtrl(btnRead);

	edX->Text = "";
	edY->Text = "";
	edZ->Text = "";

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = GetAddress();

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending expected address!";
		return;
	}

	int16_t rawValues[3];
	status = ch341a.I2CWriteCommandReadBuffer(address, L3G4200D_OUT_X_L, reinterpret_cast<uint8_t*>(rawValues), sizeof(rawValues));
	if (status != 0)
	{
		lblStatus->Caption = "Error reading raw values";
		return;
	}

	edX->Text = RawValToString(rawValues[0]);
	edY->Text = RawValToString(rawValues[1]);
	edZ->Text = RawValToString(rawValues[2]);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CL3G4200D::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------





