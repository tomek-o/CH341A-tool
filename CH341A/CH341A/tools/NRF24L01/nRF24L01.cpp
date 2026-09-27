#include "nRF24L01.h"
#include "CH341A.h"
#include "Log.h"
#include <windows.h>
#include <System.hpp>
#include <string.h>

namespace {

#define CBI(sfr, bit) ((sfr) &= static_cast<uint8_t>(~static_cast<uint8_t>(1u<<(bit))))

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
	uint8_t memoryBuffer[256];	// command byte + up to 255 data bytes
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
	uint8_t buffer[256];	// command byte + up to 255 data bytes
	buffer[0] = R_RX_PAYLOAD;
	ch341a.SpiTransfer(buffer, len+1);
	memcpy(buf, buffer+1, len);
}

bool nRfIsStatusValid(uint8_t status)
{
	return (status & 0x80) == 0;	// STATUS bit 7 is reserved and always reads 0
}

uint8_t nRfIsRXempty(void)
{
	uint8_t fifoStatus = nRfRead_register(FIFO_STATUS);
	// FIFO_STATUS bits 7 and 2..3 are reserved (read 0): anything else means
	// MISO is floating / stuck (no module), so report "empty" rather than let
	// callers read garbage payloads forever
	if (fifoStatus & 0x8C)
		return 1;
	return( fifoStatus & static_cast<uint8_t>(1<<RX_EMPTY) );      //Check FIFO status
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

namespace {

/** Read back registers written during init: SPI transfers do not report a
	missing / miswired module (MISO just reads all 0x00 or 0xFF), so this is the
	only way to tell that the chip is actually there.
*/
int VerifyInit(uint8_t aw_value, uint8_t channel, uint8_t rfSetup)
{
	// RF_SETUP bit 0 is LNA_HCURR on nRF24L01 but obsolete/don't-care on
	// nRF24L01+, so it is not compared. The RF_PWR bits make the expected
	// value nonzero, which catches a MISO stuck at 0x00 even when SETUP_AW and
	// RF_CH are legitimately 0 (2-byte sniffer width on channel 0).
	const uint8_t RF_SETUP_MASK = 0x2E;	// RF_DR_LOW, RF_DR_HIGH, RF_PWR
	uint8_t aw = nRfRead_register(SETUP_AW);
	uint8_t ch = nRfRead_register(RF_CH);
	uint8_t rs = nRfRead_register(RF_SETUP);
	if (aw != aw_value || ch != channel || (rs & RF_SETUP_MASK) != (rfSetup & RF_SETUP_MASK))
	{
		LOG("nrf24: register readback mismatch (SETUP_AW = 0x%02X, expected 0x%02X; RF_CH = 0x%02X, expected 0x%02X; RF_SETUP = 0x%02X, expected 0x%02X) - module not connected?\n",
			aw, aw_value, ch, channel, rs, rfSetup);
		return -2;
	}
	return 0;
}

}	// namespace

int nRfInitProm(  uint8_t addrWidth, uint8_t channel, uint8_t rfSpeed, const uint8_t *rxAddr ){
    // datasheet says awValue: 0=illegal, 1=3bytes, 2=4bytes, 3=5bytes;
    // the "illegal" 2-byte width is used deliberately here for promiscuous sniffing
    uint8_t aw_value = static_cast<uint8_t>(addrWidth - 2);
    if ( aw_value>3 ){
        LOG("error: addrWidth must be 2, 3, 4 or 5!\n");
        return -1;
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
    uint8_t rfSetup = static_cast<uint8_t>(0x07 | rfSpeed);
    nRfWrite_register( RF_SETUP,    rfSetup );   //selected data rate, 0dBm power, LNA_HCURR=1
    nRfWrite_register( RX_PW_P0,    32 );        //32 byte static RX payload length
    nRfWrite_register( DYNPD,        0x00 );     //Disable dynamic payload length on all pipes
    nRfWrite_register( FEATURE,     0x00 );      //Disable: Dynamic payload length, Ack payload, Dynamic noack
    // Registers may only be written in power down / standby, i.e. before CE goes high
    nRfWrite_registers( RX_ADDR_P0, rxAddr, addrWidth );
    NRF_PWR_UP();
    Sleep( 3 );                              //Wait for Powerup
    NRF_RX_MODE();
    nRfFlush_tx();
    nRfFlush_rx();
    nRfWrite_register( STATUS, static_cast<uint8_t>((1<<TX_DS) | (1<<MAX_RT) | (1<<RX_DR)) );	// clear flags
    int status = VerifyInit(aw_value, channel, rfSetup);
    if (status == 0)
        NRF_CE_ON();                         // start listening
    return status;
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

int nRfInit(uint8_t addrWidth, uint8_t channel, uint8_t rfSpeed, uint8_t payloadLength, bool enableAutoAck)
{
	// datasheet: 0=illegal, 1=3bytes, 2=4bytes, 3=5bytes
	if (addrWidth < 3 || addrWidth > 5)
	{
		LOG("nrf24: error: addrWidth must be 3, 4 or 5!\n");
		return -1;
	}
	uint8_t aw_value = static_cast<uint8_t>(addrWidth - 2);
	if (payloadLength < 1 || payloadLength > 32)
	{
		LOG("nrf24: error: payloadLength must be 1-32!\n");
		return -1;
	}

	NRF_CE_OFF();
	Sleep(5);

	cacheCONFIG = static_cast<uint8_t>((1<<EN_CRC) | (1<<CRCO));	// CRC enabled (2 bytes), power down, PRX mode bit cleared for now
	nRfWrite_register( CONFIG,     cacheCONFIG );
	nRfWrite_register( EN_AA,      static_cast<uint8_t>(enableAutoAck ? 0x01 : 0x00) );	// auto-ack on pipe 0 only
	nRfWrite_register( EN_RXADDR,  0x01 );							// enable pipe 0
	nRfWrite_register( SETUP_AW,   aw_value );
	nRfWrite_register( SETUP_RETR, static_cast<uint8_t>(enableAutoAck ? 0x2F : 0x00) );	// 750us retry delay, up to 15 retries
	nRfWrite_register( RF_CH,      channel );
	uint8_t rfSetup = static_cast<uint8_t>(0x07 | rfSpeed);
	nRfWrite_register( RF_SETUP,   rfSetup );						// 0dBm power + selected air data rate
	nRfWrite_register( RX_PW_P0,   payloadLength );
	nRfWrite_register( DYNPD,      0x00 );							// disable dynamic payload length
	nRfWrite_register( FEATURE,    0x00 );							// disable dynamic payload/ack payload/dynamic-noack

	NRF_PWR_UP();
	Sleep(3);														// wait for power-up
	return VerifyInit(aw_value, channel, rfSetup);
}

void nRfInitTX(const uint8_t *txAddr, uint8_t addrWidth)
{
    // Must leave RX mode before changing PRIM_RX.
	NRF_CE_OFF();
	
	NRF_TX_MODE();

	// Write back refreshed CONFIG register
	nRfWrite_register(CONFIG, cacheCONFIG);

	nRfWrite_registers( TX_ADDR,    txAddr, addrWidth );
	nRfWrite_registers( RX_ADDR_P0, txAddr, addrWidth );	// pipe 0 must match TX_ADDR to receive the auto-ack
	nRfFlush_tx();
	nRfFlush_rx();
	nRfWrite_register( STATUS, static_cast<uint8_t>((1<<TX_DS) | (1<<MAX_RT) | (1<<RX_DR)) );	// clear flags
}

void nRfInitRX(const uint8_t *rxAddr, uint8_t addrWidth)
{
	NRF_RX_MODE();
	nRfWrite_registers( RX_ADDR_P0, rxAddr, addrWidth );
	nRfFlush_tx();
	nRfFlush_rx();
	nRfWrite_register( STATUS, static_cast<uint8_t>((1<<TX_DS) | (1<<MAX_RT) | (1<<RX_DR)) );	// clear flags
	NRF_CE_ON();	// start listening
}

enum NrfSendResult nRfSendBytes(const uint8_t *bytesToSend, uint8_t len)
{
	uint8_t buffer[33];
	if (len < 1 || len > 32)
	{
		LOG("nrf24: error: payload length must be 1-32!\n");
		return NRF_SEND_FAILED;
	}
	buffer[0] = W_TX_PAYLOAD;
	memcpy(buffer+1, bytesToSend, len);
	ch341a.SpiTransfer(buffer, static_cast<unsigned int>(len) + 1);

	NRF_CE_ON();

	// Guarantee CE high before the first CSN assertion.
	Sleep(1);

	// Once triggered, the on-chip Enhanced ShockBurst state machine runs the whole
	// send-and-wait-for-ack(-and-retry) sequence autonomously; CE only needs to stay
	// high long enough to start it (bringing it low again does not abort a
	// retransmit sequence already in progress), so we can safely poll STATUS here
	// and drop CE as soon as we see a result.
	enum NrfSendResult result = NRF_SEND_TIMEOUT;
	DWORD start = GetTickCount();
	while ((GetTickCount() - start) < 200)
	{
		uint8_t status = nRfGet_status();
		if (!nRfIsStatusValid(status))
		{
			// MISO floating high (no module): 0xFF would otherwise read as TX_DS
			break;
		}
		if (status & (1<<TX_DS))
		{
			result = NRF_SEND_OK;
			break;
		}
		if (status & (1<<MAX_RT))
		{
			result = NRF_SEND_FAILED;
			break;
		}
		Sleep(1);
	}

	NRF_CE_OFF();
	if (result != NRF_SEND_OK)
	{
		// An unacknowledged payload is NOT removed from the TX FIFO: without a
		// flush the next send would first retransmit this stale one, and once
		// the 3-level FIFO is full new payloads would be silently dropped.
		nRfFlush_tx();
	}
	nRfWrite_register( STATUS, static_cast<uint8_t>((1<<TX_DS) | (1<<MAX_RT)) );	// clear flags for next send
	return result;
}

namespace {

const char* OnOff(uint8_t value, int bit)
{
	return (value & (1u << bit)) ? "on" : "off";
}

/** Pipe bitmask as "P0 P1 ..." or "none" */
AnsiString PipeList(uint8_t value)
{
	AnsiString s;
	for (int i=0; i<6; i++)
	{
		if (value & (1u << i))
			s.cat_printf("P%d ", i);
	}
	if (s == "")
		s = "none";
	return s.Trim();
}

void DumpLine(uint8_t reg, const char *name, uint8_t value, const AnsiString &description)
{
	LOG("  %02X %-11s = %02X  %s\n", reg, name, value, description.c_str());
}

void DumpAddress(uint8_t reg, const char *name, uint8_t len, const char *description)
{
	uint8_t buf[6];
	memset(buf, 0, sizeof(buf));
	buf[0] = static_cast<uint8_t>(R_REGISTER | (REGISTER_MASK & reg));
	ch341a.SpiTransfer(buf, static_cast<unsigned int>(len) + 1);
	AnsiString hex;
	for (int i=1; i<=len; i++)
		hex.cat_printf("%02X ", buf[i]);
	LOG("  %02X %-11s = %s %s\n", reg, name, hex.c_str(), description);
}

}	// namespace

void nRfDumpRegisters(void)
{
	AnsiString d;
	uint8_t v;

	LOG("nrf24: register dump (addresses shown LSByte first, as written over SPI)\n");

	v = nRfRead_register(CONFIG);
	d.sprintf("%s, %s, CRC %s", (v & (1<<PWR_UP)) ? "power up" : "power down",
		(v & (1<<PRIM_RX)) ? "PRX (receiver)" : "PTX (transmitter)",
		(v & (1<<EN_CRC)) ? ((v & (1<<CRCO)) ? "2 B" : "1 B") : "off");
	d.cat_printf(", IRQ masked: RX_DR %s, TX_DS %s, MAX_RT %s",
		OnOff(v, MASK_RX_DR), OnOff(v, MASK_TX_DS), OnOff(v, MASK_MAX_RT));
	DumpLine(CONFIG, "CONFIG", v, d);

	v = nRfRead_register(EN_AA);
	DumpLine(EN_AA, "EN_AA", v, "auto-ack on: " + PipeList(v));

	v = nRfRead_register(EN_RXADDR);
	DumpLine(EN_RXADDR, "EN_RXADDR", v, "RX pipes enabled: " + PipeList(v));

	v = nRfRead_register(SETUP_AW);
	if ((v & 0x03) == 0)
		d = "address width: 0 = illegal (2 B, sniffer trick)";
	else
		d.sprintf("address width: %d B", (v & 0x03) + 2);
	DumpLine(SETUP_AW, "SETUP_AW", v, d);
	uint8_t aw = static_cast<uint8_t>((v & 0x03) ? (v & 0x03) + 2 : 2);

	v = nRfRead_register(SETUP_RETR);
	if ((v & 0x0F) == 0)
		d.sprintf("auto-retransmit off (delay %u us)", ((v >> ARD) + 1) * 250);
	else
		d.sprintf("retransmit delay %u us, up to %u retries", ((v >> ARD) + 1) * 250, v & 0x0F);
	DumpLine(SETUP_RETR, "SETUP_RETR", v, d);

	v = nRfRead_register(RF_CH);
	d.sprintf("channel %u = %u MHz", v & 0x7F, 2400 + (v & 0x7F));
	DumpLine(RF_CH, "RF_CH", v, d);

	v = nRfRead_register(RF_SETUP);
	{
		const char *rate;
		if (v & (1<<RF_DR_LOW))
			rate = (v & (1<<RF_DR_HIGH)) ? "reserved" : "250 kbps";
		else
			rate = (v & (1<<RF_DR_HIGH)) ? "2 Mbps" : "1 Mbps";
		static const char* const power[] = { "-18 dBm", "-12 dBm", "-6 dBm", "0 dBm" };
		d.sprintf("%s, %s%s%s", rate, power[(v >> 1) & 0x03],
			(v & (1<<7)) ? ", CONT_WAVE" : "", (v & (1<<PLL_LOCK)) ? ", PLL_LOCK" : "");
	}
	DumpLine(RF_SETUP, "RF_SETUP", v, d);

	v = nRfRead_register(STATUS);
	if (!nRfIsStatusValid(v))
	{
		d = "INVALID (reserved bit 7 set) - module not connected?";
	}
	else
	{
		uint8_t pipe = static_cast<uint8_t>((v >> RX_P_NO) & 0x07);
		if (pipe == 7)
			d = "RX FIFO empty";
		else if (pipe == 6)
			d = "RX pipe: unused value";
		else
			d.sprintf("next RX payload from P%u", pipe);
		if (v & (1<<RX_DR)) d += ", RX_DR (data received)";
		if (v & (1<<TX_DS)) d += ", TX_DS (sent)";
		if (v & (1<<MAX_RT)) d += ", MAX_RT (retries exhausted)";
		if (v & (1<<TX_FULL)) d += ", TX FIFO full";
	}
	DumpLine(STATUS, "STATUS", v, d);

	v = nRfRead_register(OBSERVE_TX);
	d.sprintf("lost packets %u (since RF_CH write), retransmits of last packet %u", v >> PLOS_CNT, v & 0x0F);
	DumpLine(OBSERVE_TX, "OBSERVE_TX", v, d);

	v = nRfRead_register(CD);
	DumpLine(CD, "RPD/CD", v, (v & 0x01) ? "carrier > -64 dBm detected" : "no carrier detected");

	// P0, P1 and TX_ADDR are full-width; P2..P5 only hold the LSByte (rest shared with P1)
	DumpAddress(RX_ADDR_P0, "RX_ADDR_P0", aw, "");
	DumpAddress(RX_ADDR_P1, "RX_ADDR_P1", aw, "");
	DumpAddress(RX_ADDR_P2, "RX_ADDR_P2", 1, "(LSByte, rest = P1)");
	DumpAddress(RX_ADDR_P3, "RX_ADDR_P3", 1, "(LSByte, rest = P1)");
	DumpAddress(RX_ADDR_P4, "RX_ADDR_P4", 1, "(LSByte, rest = P1)");
	DumpAddress(RX_ADDR_P5, "RX_ADDR_P5", 1, "(LSByte, rest = P1)");
	DumpAddress(TX_ADDR, "TX_ADDR", aw, "");

	static const char* const pwNames[] = { "RX_PW_P0", "RX_PW_P1", "RX_PW_P2", "RX_PW_P3", "RX_PW_P4", "RX_PW_P5" };
	for (int i=0; i<6; i++)
	{
		uint8_t reg = static_cast<uint8_t>(RX_PW_P0 + i);
		v = nRfRead_register(reg);
		if ((v & 0x3F) == 0)
			d.sprintf("pipe %d payload length: 0 = pipe not used", i);
		else
			d.sprintf("pipe %d payload length: %u B", i, v & 0x3F);
		DumpLine(reg, pwNames[i], v, d);
	}

	v = nRfRead_register(FIFO_STATUS);
	d.sprintf("TX FIFO %s, RX FIFO %s%s",
		(v & (1<<FIFO_FULL)) ? "full" : ((v & (1<<TX_EMPTY)) ? "empty" : "has data"),
		(v & (1<<RX_FULL)) ? "full" : ((v & (1<<RX_EMPTY)) ? "empty" : "has data"),
		(v & (1<<TX_REUSE)) ? ", TX_REUSE" : "");
	DumpLine(FIFO_STATUS, "FIFO_STATUS", v, d);

	v = nRfRead_register(DYNPD);
	DumpLine(DYNPD, "DYNPD", v, "dynamic payload length on: " + PipeList(v));

	v = nRfRead_register(FEATURE);
	d.sprintf("dynamic payload length %s, ACK payload %s, W_TX_PAYLOAD_NOACK %s",
		OnOff(v, EN_DPL), OnOff(v, EN_ACK_PAY), OnOff(v, EN_DYN_ACK));
	DumpLine(FEATURE, "FEATURE", v, d);
}

uint8_t nRfGetRetransmits(void)
{
	return static_cast<uint8_t>(nRfRead_register(OBSERVE_TX) & 0x0F);
}

bool nRfIsTXempty(void)
{
	return (nRfRead_register(FIFO_STATUS) & (1<<TX_EMPTY)) != 0;
}
