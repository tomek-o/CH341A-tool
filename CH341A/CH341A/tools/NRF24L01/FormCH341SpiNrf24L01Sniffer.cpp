//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SpiNrf24L01Sniffer.h"
#include "CH341A.h"
#include "nRF24L01.h"
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

#define RX_PROMISCUOUS_LENGTH 32       // fixed payload length 

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

	{
		int status;
		AnsiString msg;
		std::vector<uint8_t> data;

		status = HexStringCleanToBuf(edAddress->Text, msg, data);
		if (status != 0)
		{
			MessageBox(this->Handle, "Failed to convert text to data to write", Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}

		if (static_cast<int>(data.size()) < addressBytes)
		{
			MessageBox(this->Handle, "Number of hex address bytes is smaller than selected", Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}

		for (int i=0; i<addressBytes; i++)
		{
        	addr[i] = data[i];
		}
	}

	int TODO__FIX_SPEED;
	nRfInitProm( addressBytes, static_cast<uint8_t>(cbRfChannel->ItemIndex), cbRfSpeed->ItemIndex == 2 );
	nRfWrite_registers( RX_ADDR_P0,addr, addressBytes );//write the address to nRF register
    nRfFlush_tx();
    nRfFlush_rx();
	nRfWrite_register( STATUS, (1<<RX_DR) );		        //Clear Data ready flag

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

	for (unsigned int repeat = 0; repeat < 5; repeat++) {
        if( nRfIsDataReceived() ){
			while( !nRfIsRXempty() ){        //Readout and empty the RX FIFO
                uint8_t recBuffer[255];
				nRfRead_payload( recBuffer, RX_PROMISCUOUS_LENGTH );
				AnsiString text = "nrf24 RX: ";
				for (int i=0; i<RX_PROMISCUOUS_LENGTH; i++)
				{
					text.cat_printf("%02X ", recBuffer[i]);
				}
				LOG("%s\n", text.c_str());
            }
            nRfWrite_register( STATUS, (1<<RX_DR) );		//Clear Data ready flag
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

