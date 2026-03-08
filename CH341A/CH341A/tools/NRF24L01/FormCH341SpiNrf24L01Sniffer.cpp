//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SpiNrf24L01Sniffer.h"
#include "CH341A.h"
#include "msprf24.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "common/bin2str.h"
#include "ValueDescription.h"
#include "Log.h"
#include <assert.h>
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SpiNrf24L01Sniffer *frmCH341SpiNrf24L01Sniffer;
//---------------------------------------------------------------------------

namespace
{

ValueDescriptionU8 rfSpeedSel[] = {
	{ RF24_SPEED_250KBPS, "250 kbps" },
	{ RF24_SPEED_1MBPS, "1 Mbps" },
	{ RF24_SPEED_2MBPS, "2 Mbps (NRF24L01+ only)" },
};

}	// namespace

__fastcall TfrmCH341SpiNrf24L01Sniffer::TfrmCH341SpiNrf24L01Sniffer(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupRadio);
	FillComboboxWithValues(rfSpeedSel, cbRfSpeed, RF24_SPEED_2MBPS);
	for (int i=0; i<=125; i++)
	{
		cbRfChannel->Items->Add(i);
	}
	cbRfChannel->ItemIndex = 5;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01Sniffer::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	lblStatus->Caption = "";

	uint8_t addr[5];
	memset(addr, 0, sizeof(addr));
	uint8_t addressBytes = static_cast<uint8_t>(cbAddressBytes->ItemIndex + 2);
	msprf24_init(static_cast<uint8_t>(cbRfChannel->ItemIndex), addressBytes,
		rfSpeedSel[cbRfSpeed->ItemIndex].value | static_cast<uint8_t>(RF24_POWER_MAX));
	msprf24_set_pipe_packetsize(0, 32);		// set fixed packet size for monitor
	msprf24_open_pipe(0, 0 /* autoack */);  // Open pipe#0 with Enhanced ShockBurst (autoack disabled)

	// Set our RX address
#if 0
	addr[0] = 0x7A;	// logitech
	addr[1] = 0x77;	// logitech
	addr[2] = 0x94;
#endif

#if 0
	// matches many
	addr[0] = 0xAA;
	addr[1] = 0xAA;
	addr[2] = 0xAA;
#endif

#if 0
	// sliding address technique - less matches, better suited to find addresses
	addr[0] = 0x00;
	addr[1] = 0xAA;
	addr[2] = 0xAA;
#endif

	{
		int status;
		AnsiString msg;
		std::vector<uint8_t> data;

		status = HexStringCleanToBuf(edAddress->Text, msg, data);
		if (status != 0)
		{
			MessageBox(this->Handle, msg.c_str(), "Failed to convert text to data to write", MB_ICONEXCLAMATION);
			return;
		}

		if (static_cast<int>(data.size()) < addressBytes)
		{
			MessageBox(this->Handle, msg.c_str(), "Number of hex address bytes is smaller than selected", MB_ICONEXCLAMATION);
			return;
		}

		for (int i=0; i<addressBytes; i++)
		{
        	addr[i] = data[i];
		}
	}

	w_rx_addr(0, addr);

	// Receive mode
	if (!(RF24_QUEUE_RXEMPTY & msprf24_queue_state())) {
		flush_rx();
	}

#if 1
	msprf24_activate_rx(0 /* RF24_EN_CRC */);
#else
	msprf24_activate_rx(RF24_EN_CRC /*  | RF24_CRCO */);
#endif
	LOG("nRF24 initialized\n");
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01Sniffer::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341SpiNrf24L01Sniffer::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t buf[256];	// max uint8_t + 1

	for (unsigned int repeat = 0; repeat < 5; repeat++) {
		if (msprf24_rx_pending()) {
			uint8_t length = r_rx_peek_payload_size();
			r_rx_payload(length, buf);
			msprf24_irq_clear(RF24_IRQ_RX);
			AnsiString text = "nrf24 RX: ";
			for (int i=0; i<length; i++)
			{
				text.cat_printf("%02X ", buf[i]);
			}
			LOG("%s\n", text.c_str());
		} else {
			break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SpiNrf24L01Sniffer::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01Sniffer::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01Sniffer::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

