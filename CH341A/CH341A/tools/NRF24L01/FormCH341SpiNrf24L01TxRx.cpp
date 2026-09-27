//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SpiNrf24L01TxRx.h"
#include "CH341A.h"
#include "nRF24L01.h"
#include "TabManager.h"
#include "common/ScopedBool.h"
#include "common/bin2str.h"
#include "ValueDescription.h"
#include "Log.h"
#include <assert.h>
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SpiNrf24L01TxRx *frmCH341SpiNrf24L01TxRx;
//---------------------------------------------------------------------------

namespace
{

ValueDescriptionU8 rfSpeedSel[] = {
	{ RF24_SPEED_250KBPS, "250 kbps" },
	{ RF24_SPEED_1MBPS, "1 Mbps" },
	{ RF24_SPEED_2MBPS, "2 Mbps (NRF24L01+ only)" },
};

enum { MODE_TRANSMITTER = 0, MODE_RECEIVER = 1 };

}	// namespace

__fastcall TfrmCH341SpiNrf24L01TxRx::TfrmCH341SpiNrf24L01TxRx(TComponent* Owner)
	: TForm(Owner),
	initialized(false),
	busy(false),
	activePayloadLength(32)
{
	TabManager::Instance().Register(this, 1u << ToolGroupRadio);

	FillComboboxWithValues(rfSpeedSel, cbRfSpeed, RF24_SPEED_2MBPS);

	for (int i=0; i<=125; i++)
	{
		cbRfChannel->Items->Add(i);
	}
	cbRfChannel->ItemIndex = 5;

	for (int i=1; i<=32; i++)
	{
		cbPayloadLength->Items->Add(i);
	}
	cbPayloadLength->ItemIndex = 31;	// 32 bytes

	UpdateModeControlsEnabled();
}
//---------------------------------------------------------------------------

void TfrmCH341SpiNrf24L01TxRx::UpdateModeControlsEnabled(void)
{
	bool isTx = (rgMode->ItemIndex == MODE_TRANSMITTER);

	// Hide the inactive panel entirely (rather than just disabling it) so only
	// the relevant controls for the current mode are shown; the visible one
	// moves up into grpTx's slot so there's no dead gap above it.
	grpTx->Visible = initialized && isTx;
	grpRx->Visible = initialized && !isTx;
	if (grpRx->Visible)
	{
		grpRx->Top = grpTx->Top;
	}

	if (!grpRx->Visible)
	{
		chbRxAutoRead->Checked = false;
		tmrRxAutoRead->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01TxRx::rgModeClick(TObject *Sender)
{
	initialized = false;	// mode change requires a fresh Init
	lblStatus->Caption = "Mode changed - click Init to apply";
	UpdateModeControlsEnabled();
}
//---------------------------------------------------------------------------

bool TfrmCH341SpiNrf24L01TxRx::ParseAddress(TEdit *edit, uint8_t addressBytes, uint8_t *addr)
{
	memset(addr, 0, 5);

	AnsiString msg;
	std::vector<uint8_t> data;

	int status = HexStringCleanToBuf(edit->Text, msg, data);
	if (status != 0)
	{
		MessageBox(this->Handle, "Failed to convert address text to data", Caption.c_str(), MB_ICONEXCLAMATION);
		return false;
	}

	if (static_cast<int>(data.size()) < addressBytes)
	{
		MessageBox(this->Handle, "Number of hex address bytes is smaller than selected", Caption.c_str(), MB_ICONEXCLAMATION);
		return false;
	}

	for (int i=0; i<addressBytes; i++)
	{
		addr[i] = data[i];
	}
	return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01TxRx::btnInitClick(TObject *Sender)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	lblStatus->Caption = "";
	initialized = false;
	UpdateModeControlsEnabled();

	bool isTx = (rgMode->ItemIndex == MODE_TRANSMITTER);
	uint8_t addressBytes = static_cast<uint8_t>(cbAddressBytes->ItemIndex + 3);	// list: 3, 4, 5

	uint8_t addr[5];
	if (!ParseAddress(isTx ? edTxAddress : edRxAddress, addressBytes, addr))
	{
		return;
	}

	uint8_t payloadLength = static_cast<uint8_t>(cbPayloadLength->ItemIndex + 1);
	uint8_t rfSpeed = rfSpeedSel[cbRfSpeed->ItemIndex].value;
	uint8_t channel = static_cast<uint8_t>(cbRfChannel->ItemIndex);

	int status = nRfInit(addressBytes, channel, rfSpeed, payloadLength, chbAutoAck->Checked);
	if (status == -2)
	{
		lblStatus->Caption = "nRF24L01 not responding (register readback failed) - check wiring";
		return;
	}
	else if (status != 0)
	{
		lblStatus->Caption = "Invalid nRF24L01 configuration";
		return;
	}

	if (isTx)
	{
		nRfInitTX(addr, addressBytes);
		lblStatus->Caption = "Initialized as transmitter";
	}
	else
	{
		nRfInitRX(addr, addressBytes);
		lblStatus->Caption = "Initialized as receiver, listening...";
	}

	memoRxLog->Clear();
	lblTxStatus->Caption = "";
	activePayloadLength = payloadLength;
	initialized = true;
	UpdateModeControlsEnabled();

	LOG("nRF24 TX/RX tool initialized\n");
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01TxRx::btnSendClick(TObject *Sender)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	if (!initialized || rgMode->ItemIndex != MODE_TRANSMITTER)
	{
		lblTxStatus->Caption = "Not initialized as transmitter!";
		return;
	}

	uint8_t payloadLength = activePayloadLength;	// as configured at Init, not the (possibly changed) combobox

	AnsiString msg;
	std::vector<uint8_t> data;
	int status = HexStringCleanToBuf(edTxData->Text, msg, data);
	if (status != 0)
	{
		lblTxStatus->Caption = "Invalid hex data to send!";
		return;
	}

	// Pad with zeros (or truncate) to the fixed payload length configured at Init.
	uint8_t buffer[32];
	memset(buffer, 0, sizeof(buffer));
	unsigned int copyCount = (data.size() < static_cast<unsigned int>(payloadLength)) ? static_cast<unsigned int>(data.size()) : payloadLength;
	for (unsigned int i=0; i<copyCount; i++)
	{
		buffer[i] = data[i];
	}

	enum NrfSendResult result = nRfSendBytes(buffer, payloadLength);

	AnsiString text;
	switch (result)
	{
	case NRF_SEND_OK:
		text = "Sent OK (ACK received)";
		break;
	case NRF_SEND_FAILED:
		text = "Send failed (no ACK, max retries reached)";
		break;
	default:
		text = "Send timed out (no response from chip)";
		break;
	}
	text.cat_printf(", retransmits used: %u", static_cast<unsigned int>(nRfGetRetransmits()));
	lblTxStatus->Caption = text;
}
//---------------------------------------------------------------------------

void TfrmCH341SpiNrf24L01TxRx::RxRead(void)
{
	if (!ch341a.IsOpened() || !initialized || rgMode->ItemIndex != MODE_RECEIVER)
		return;
	if (busy)
		return;
	ScopedBool guard(&busy);

	uint8_t payloadLength = activePayloadLength;	// must match RX_PW_P0 set at Init

	// Poll the FIFO itself, not the RX_DR flag: clearing RX_DR after draining
	// can also clear the flag of a packet that arrived in between, which then
	// sat unseen in the FIFO until the next one came. Upper bound so a busy
	// channel cannot keep this loop (and the UI) spinning.
	for (unsigned int count = 0; count < 16 && !nRfIsRXempty(); count++)
	{
		uint8_t recBuffer[32];
		nRfRead_payload(recBuffer, payloadLength);
		nRfWrite_register(STATUS, static_cast<uint8_t>(1<<RX_DR));	// clear Data Ready flag
		std::string hex = BufToSpaceSeparatedHexString(recBuffer, payloadLength);
		memoRxLog->Lines->Add(hex.c_str());
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01TxRx::btnRxReadClick(TObject *Sender)
{
	RxRead();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01TxRx::tmrRxAutoReadTimer(TObject *Sender)
{
	tmrRxAutoRead->Enabled = false;
	if (chbRxAutoRead->Checked)
		RxRead();
	tmrRxAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01TxRx::chbRxAutoReadClick(TObject *Sender)
{
	tmrRxAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01TxRx::btnDumpRegistersClick(TObject *Sender)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	nRfDumpRegisters();
	lblStatus->Caption = "Register dump written to log";
}
//---------------------------------------------------------------------------
