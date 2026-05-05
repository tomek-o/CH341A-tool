#include "nRF24L01.h"
#include "CH341A.h"
#include "Log.h"

namespace {

enum
{
	CE_PIN_ID = 8,	// CE = TXD, output only
};

int NRF_CE_OFF(void) {
	int status = ch341a.SetGpioOutputs(1u << CE_PIN_ID, 0x00000000);
	if (status != 0)
	{
		LOG("nrf24: failed to clear CE pin\n");
	}
	return status;
}

int NRF_CE_ON(void) {
	int status = ch341a.SetGpioOutputs(1u << CE_PIN_ID, 1u << CE_PIN_ID);
	if (status != 0)
	{
		LOG("nrf24: failed to set CE pin\n");
	}
	return status;
}

uint8_t cacheCONFIG = 0;

#define NRF_PWR_UP()   { cacheCONFIG |= (1<<PWR_UP);  nRfWrite_register( CONFIG, cacheCONFIG ); }
#define NRF_PWR_DOWN() { CBI( cacheCONFIG, PWR_UP);  nRfWrite_register( CONFIG, cacheCONFIG ); }
#define NRF_RX_MODE()  { cacheCONFIG |= (1<<PRIM_RX); nRfWrite_register( CONFIG, cacheCONFIG ); }
#define NRF_TX_MODE()  { CBI( cacheCONFIG, PRIM_RX); nRfWrite_register( CONFIG, cacheCONFIG ); }

}

void nRfWrite_registers(uint8_t reg, const uint8_t* buf, uint8_t len) {
	uint8_t memoryBuffer[255];
	memoryBuffer[0] = static_cast<uint8_t>(W_REGISTER | ( REGISTER_MASK & reg ));
	memcpy(memoryBuffer + 1, buf, len);
	ch341a.SpiTransfer(memoryBuffer, len+1);
}

bool nRfIsDataReceived(void)
{
	return (nRfGet_status() & (1<<RX_DR));
}

uint8_t nRfGet_status(void){
	uint8_t buf[1];
	buf[0] = NOP;
	ch341a.SpiTransfer(buf, 1);
	return buf[0];
}

void nRfRead_payload(void* buf, uint8_t len) {
	uint8_t buffer[256];
	buffer[0] = R_RX_PAYLOAD;
	ch341a.SpiTransfer(buffer, len+1);
	memcpy(buf, buffer+1, len);
}

uint8_t nRfIsRXempty(void)
{
	return( nRfRead_register(FIFO_STATUS) & static_cast<uint8_t>(1<<RX_EMPTY) );      //Check FIFO status
}

uint8_t nRfRead_register( uint8_t reg )
{
	uint8_t buffer[2];
	buffer[0] = static_cast<uint8_t>(R_REGISTER | ( REGISTER_MASK & reg ));
	ch341a.SpiTransfer(buffer, 2);
	return buffer[1];
}

void nRfWrite_register(uint8_t reg, uint8_t value) {
	uint8_t buffer[2];
	buffer[0] = static_cast<uint8_t>(W_REGISTER | ( REGISTER_MASK & reg ));
	buffer[1] = value;
	ch341a.SpiTransfer(buffer, 2);
}

void nRfInitProm(  uint8_t addrWidth, uint8_t channel, uint8_t is2mbpsRate ){
    uint8_t temp, aw_value = static_cast<uint8_t>(addrWidth - 2);         // datasheet says awValue: 0=illegal, 1=3bytes, 2=4bytes, 3=5bytes
    if ( aw_value>3 ){
        LOG("error: addrWidth must be 2, 3, 4 or 5!\n");
        return;
    }
                             
    NRF_CE_OFF();

    Sleep(5);
    cacheCONFIG = 0x71;                    //3xIRQs OFF, No CRC, Power Down, PRX mode
    nRfWrite_register( CONFIG,    cacheCONFIG );
    nRfWrite_register( EN_AA,     0x00 );        //Disable auto ACK on pipe0 - pipe5
    nRfWrite_register( EN_RXADDR, 0x01 );  //Enable ERX_P0 pipe
    nRfWrite_register( SETUP_AW,  aw_value );    //n bytes address width !
    nRfWrite_register( SETUP_RETR,0x00 );        //Automatic retransmit disabled
    nRfWrite_register( RF_CH,     channel );     //Set RF channel to x
    temp = 0x07;
    if( is2mbpsRate ){
		temp |= (1<<3);
    }   
    nRfWrite_register( RF_SETUP,    temp );      //1 or 2 Mbps data rate, 0dBm power, LNA_HCURR=1
    nRfWrite_register( RX_PW_P0,    32 );        //32 byte static RX payload length
    nRfWrite_register( DYNPD,        0x00 );     //Disable dynamic payload length on all pipes
    nRfWrite_register( FEATURE,     0x00 );      //Disable: Dynamic payload length, Ack payload, Dynamic noack
    NRF_PWR_UP();
    Sleep( 3 );                              //Wait for Powerup
    NRF_RX_MODE();
    NRF_CE_ON();
}

void nRfFlush_rx(void){
	uint8_t buffer[1];
	buffer[0] = FLUSH_RX;
	ch341a.SpiTransfer(buffer, 1);
}

void nRfFlush_tx(void){
	uint8_t buffer[1];
	buffer[0] = FLUSH_TX;
	ch341a.SpiTransfer(buffer, 1);
}
