#ifndef nRF24L01H
#define nRF24L01H

#include "nRF24L01_regs.h"
#include <stdint.h>

void nRfInitProm(  uint8_t addrWidth, uint8_t channel, uint8_t is2mbpsRate );
void nRfInitTX();
void nRfInitRX();
void nRfSendBytes( uint8_t *bytesToSend, uint8_t len, uint8_t *adr,  uint8_t noAck );
void nRfSendAccPayload( uint8_t *bytesToSend, uint8_t len, uint8_t pipeNumber );
uint8_t nRfIsDataSent();
bool nRfIsDataReceived();
uint8_t nRfGetRetransmits();
uint8_t nRfIsRXempty();
uint8_t nRfIsTXempty();
void nRfHandleISR();

void nRfWrite_register(uint8_t reg, uint8_t value);
void nRfWrite_registers(uint8_t reg, const uint8_t* buf, uint8_t len);
void nRfFlush_rx(void);
void nRfFlush_tx(void);
uint8_t nRfGet_status(void);
uint8_t nRfRead_register( uint8_t reg );
void nRfRead_payload(void* buf, uint8_t len);

#endif