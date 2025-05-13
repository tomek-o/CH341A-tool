//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SpiNrf24L01.h"
#include "CH341A.h"
#include "msprf24.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "ValueDescription.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SpiNrf24L01 *frmCH341SpiNrf24L01;
//---------------------------------------------------------------------------

namespace
{

ValueDescriptionU8 rfSpeedSel[] = {
	{ RF24_SPEED_250KBPS, "250 kbps" },
	{ RF24_SPEED_1MBPS, "1 Mbps" },
	{ RF24_SPEED_2MBPS, "2 Mbps (NRF24L01+ only)" },
};

}	// namespace

__fastcall TfrmCH341SpiNrf24L01::TfrmCH341SpiNrf24L01(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
	FillComboboxWithValues(rfSpeedSel, cbRfSpeed, RF24_SPEED_2MBPS);
	for (int i=0; i<=125; i++)
	{
		cbRfChannel->Items->Add(i);
	}
	cbRfChannel->ItemIndex = 5;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01::btnInitClick(TObject *Sender)
{
	BtnController btnCtrl(btnInit);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	rf_crc = 0; //RF24_EN_CRC; // CRC enabled, 8-bit
	uint8_t addr[5];
	enum { ADDR_LEN = 2 };
	rf_addr_width      = ADDR_LEN;
	rf_speed_power     = rfSpeedSel[cbRfSpeed->ItemIndex].value | static_cast<uint8_t>(RF24_POWER_MAX);
	rf_channel         = static_cast<uint8_t>(cbRfChannel->ItemIndex);
	msprf24_init();
	int TODO__SET_FIXED_PACKET_SIZE_FOR_MONITOR;
	msprf24_set_pipe_packetsize(0, 32);
	int TODO__DISABLE_AUTOACK_FOR_MONITOR;
	msprf24_open_pipe(0, 0 /* autoack */);  // Open pipe#0 with Enhanced ShockBurst (autoack)

	// Set our RX address
	memcpy(addr, "\xDE\xAD", ADDR_LEN);
	w_rx_addr(0, addr);
#if 0
	// Receive mode
	if (!(RF24_QUEUE_RXEMPTY & msprf24_queue_state())) {
		flush_rx();
	}
#endif
	msprf24_activate_rx();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341SpiNrf24L01::Read(void)
{
	BtnController btnCtrl(btnRead);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t buf[32];

	if (msprf24_rx_pending()) {
		r_rx_payload(r_rx_peek_payload_size(), buf);
		msprf24_irq_clear(RF24_IRQ_RX);
		LOG("nrf24: Got RX data packet\n");
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SpiNrf24L01::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01::chbAutoReadClick(TObject *Sender)
{
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiNrf24L01::chbAutoReadMouseDown(TObject *Sender,
	  TMouseButton Button, TShiftState Shift, int X, int Y)
{
	tmrAutoRead->Enabled = true;	
}
//---------------------------------------------------------------------------

