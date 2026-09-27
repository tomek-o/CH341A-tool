#ifndef nRF24L01H
#define nRF24L01H

#include "nRF24L01_regs.h"
#include <stdint.h>

/** Result of a single nRfSendBytes() attempt */
enum NrfSendResult
{
	NRF_SEND_OK,		///< ACK received (or auto-ack disabled and the payload simply went out)
	NRF_SEND_FAILED,	///< auto-retransmit limit (MAX_RT) reached without an ACK
	NRF_SEND_TIMEOUT	///< neither TX_DS nor MAX_RT was seen before the wait timed out
};

/** \brief Promiscuous (sniffer) RX setup: no CRC, no auto-ack, 32-byte payload.
	\param addrWidth 2-5; 2 is the datasheet-illegal width used for sniffing
	\param rfSpeed one of RF24_SPEED_250KBPS/RF24_SPEED_1MBPS/RF24_SPEED_2MBPS
	\param rxAddr addrWidth bytes, written LSByte first; set before listening starts
	\return 0 on success (listening), -1 on invalid argument, -2 if register readback failed (no module?)
*/
int nRfInitProm(uint8_t addrWidth, uint8_t channel, uint8_t rfSpeed, const uint8_t *rxAddr);

/** \brief False if STATUS has its reserved bit 7 set, i.e. MISO floating high (no module) */
bool nRfIsStatusValid(uint8_t status);

/** \brief Common transceiver setup, shared by TX and RX modes.
	\param rfSpeed one of RF24_SPEED_250KBPS/RF24_SPEED_1MBPS/RF24_SPEED_2MBPS
	\param payloadLength fixed payload length used on pipe 0, 1-32 bytes
	\param enableAutoAck enable Enhanced ShockBurst auto-acknowledge + auto-retransmit on pipe 0
	\param addrWidth 3-5 bytes
	\return 0 on success, -1 on invalid argument, -2 if register readback failed (no module?)
*/
int nRfInit(uint8_t addrWidth, uint8_t channel, uint8_t rfSpeed, uint8_t payloadLength, bool enableAutoAck);

/** \brief Switch to transmitter mode: sets TX_ADDR (and, for auto-ack, RX_ADDR_P0) to txAddr, then idles with CE low */
void nRfInitTX(const uint8_t *txAddr, uint8_t addrWidth);

/** \brief Switch to receiver mode: sets RX_ADDR_P0 to rxAddr and starts listening (CE high) */
void nRfInitRX(const uint8_t *rxAddr, uint8_t addrWidth);

/** \brief Load and send one payload, waiting (with a timeout) for the on-air result.
	\param len must match the payloadLength configured via nRfInit()
*/
enum NrfSendResult nRfSendBytes(const uint8_t *bytesToSend, uint8_t len);

/** \brief Write all registers, with short decoded descriptions, to the log.
	Read-only; works without prior init (useful for wiring / clone diagnostics).
*/
void nRfDumpRegisters(void);

uint8_t nRfGetRetransmits(void);
bool nRfIsTXempty(void);

bool nRfIsDataReceived();
uint8_t nRfIsRXempty();

void nRfWrite_register(uint8_t reg, uint8_t value);
void nRfWrite_registers(uint8_t reg, const uint8_t* buf, uint8_t len);
void nRfFlush_rx(void);
void nRfFlush_tx(void);
uint8_t nRfGet_status(void);
uint8_t nRfRead_register( uint8_t reg );
void nRfRead_payload(void* buf, uint8_t len);

#endif