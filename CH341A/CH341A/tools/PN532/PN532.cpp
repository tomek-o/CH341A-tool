/*!
    @section intro_sec Introduction

    Driver for NXP's PN532 NFC/13.56MHz RFID Transceiver

    This is a library for the Adafruit PN532 NFC/RFID breakout boards
    This library works with the Adafruit NFC breakout
    ----> https://www.adafruit.com/products/364

    Check out the links above for our tutorials and wiring diagrams
    These chips use SPI or I2C to communicate.

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section author Author

    Adafruit Industries

    @section license License

    BSD (see license.txt)

    @section  HISTORY

    v2.2 - Added startPassiveTargetIDDetection() to start card detection and
            readDetectedPassiveTargetID() to read it, useful when using the
            IRQ pin.

    v2.1 - Added NTAG2xx helper functions

    v2.0 - Refactored to add I2C support from Adafruit_NFCShield_I2C library.

    v1.4 - Added setPassiveActivationRetries()

    v1.2 - Added writeGPIO()
         - Added readGPIO()

    v1.1 - Changed readPassiveTargetID() to handle multiple UID sizes
         - Added the following helper functions for text display
             static void PrintHex(const byte * data, const uint32_t numBytes)
             static void PrintHexChar(const byte * pbtData, const uint32_t
   numBytes)
         - Added the following Mifare Classic functions:
             bool mifareclassic_IsFirstBlock (uint32_t uiBlock)
             bool mifareclassic_IsTrailerBlock (uint32_t uiBlock)
             uint8_t mifareclassic_AuthenticateBlock (uint8_t * uid, uint8_t
   uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t * keyData) uint8_t
   mifareclassic_ReadDataBlock (uint8_t blockNumber, uint8_t * data) uint8_t
   mifareclassic_WriteDataBlock (uint8_t blockNumber, uint8_t * data)
         - Added the following Mifare Ultalight functions:
             uint8_t mifareultralight_ReadPage (uint8_t page, uint8_t * buffer)
*/
/**************************************************************************/

#include "PN532.h"
#include "common/bin2str.h"
#include "CH341A.h"
#include "CH341SoftwareI2C.h"
#include "Log.h"
#include <windows.h>
#include <vector>
#include <algorithm>

#ifdef __BORLANDC__
#pragma warn -8071
#endif

uint8_t pn532ack[] = {0x00, 0x00, 0xFF,
				   0x00, 0xFF, 0x00}; ///< ACK message from PN532
uint8_t pn532response_firmwarevers[] = {
    0x00, 0x00, 0xFF,
    0x06, 0xFA, 0xD5}; ///< Expected firmware version message from PN532

// Uncomment these lines to enable debug output for PN532(SPI) and/or MIFARE
// related code

// #define PN532DEBUG
// #define MIFAREDEBUG

// If using Native Port on Arduino Zero or Due define as SerialUSB
#define PN532DEBUGPRINT Serial ///< Fixed name for debug Serial instance
// #define PN532DEBUGPRINT SerialUSB ///< Fixed name for debug Serial instance

#define PN532_PACKBUFFSIZ 64                ///< Packet buffer size in bytes
uint8_t pn532_packetbuffer[PN532_PACKBUFFSIZ]; ///< Packet buffer used in various
                                            ///< transactions

PN532::PN532(enum Interface iface):
  _iface(iface),
  _lastAtqa(0),
  _lastSak(0) {
}

namespace {
uint8_t reverseBits(uint8_t b) {
  b = (uint8_t)(((b & 0xF0) >> 4) | ((b & 0x0F) << 4));
  b = (uint8_t)(((b & 0xCC) >> 2) | ((b & 0x33) << 2));
  b = (uint8_t)(((b & 0xAA) >> 1) | ((b & 0x55) << 1));
  return b;
}
}

/*! @brief  Full-duplex SPI transfer, whole buffer within one CS assertion.
    PN532 uses LSB-first SPI while CH341 streams are configured MSB-first
    globally (at open), so bytes are bit-reversed on the way in and out.
*/
bool PN532::spiTransfer(uint8_t *buff, unsigned int n) {
  for (unsigned int i = 0; i < n; i++)
    buff[i] = reverseBits(buff[i]);
  int status = ch341a.SpiTransfer(buff, n);
  for (unsigned int i = 0; i < n; i++)
    buff[i] = reverseBits(buff[i]);
  return status == 0;
}

/**************************************************************************/
/*!
    @brief  Setups the HW

    @returns  true if successful, otherwise false
*/
/**************************************************************************/
int PN532::begin() {
  reset(); // HW reset - put in known state
  if (_iface == INTERFACE_I2C) {
    // begin() logs the specific cause (RXD-SCL loopback missing, stuck bus...)
    if (ch341SoftwareI2C.begin() != 0) {
      LOG("PN532: software I2C init failed\n");
      return -1;
    }
    ch341SoftwareI2C.setDeviceID(PN532_I2C_ADDRESS);
  }
  Sleep(10);
  return wakeup(); // hey! wakeup!
}

/**************************************************************************/
/*!
    @brief  Perform a hardware reset. Requires reset pin to have been provided.
*/
/**************************************************************************/
void PN532::reset(void) {
#if 0
  // see Datasheet p.209, Fig.48 for timings
  if (_reset != -1) {
	digitalWrite(_reset, LOW);
	delay(1); // min 20ns
	digitalWrite(_reset, HIGH);
	delay(2); // max 2ms
  }
#else
  //int TODO__RESET;
#endif
}

/**************************************************************************/
/*!
    @brief  Wakeup from LowVbat mode into Normal Mode.
*/
/**************************************************************************/
int PN532::wakeup(void) {
#if 0
  // interface specific wakeups - each one is unique!
  if (spi_dev) {
	// hold CS low for 2ms
	digitalWrite(_cs, LOW);
	delay(2);
  } else if (ser_dev) {
	uint8_t w[3] = {0x55, 0x00, 0x00};
	ser_dev->write(w, 3);
	delay(2);
  }
#endif

  // SPI: a falling CS edge wakes the PN532, but its oscillator needs a few ms
  // to start. Reference drivers hold CS low for 2 ms; the CH341 only asserts
  // CS for the duration of a transfer (microseconds), so after power-up the
  // first command can still be lost while the chip is starting. Retry the
  // wakeup + SAMConfig sequence instead of failing the first Init.
  // I2C: PN532 will clock stretch during SAMConfig as a "wakeup".
  const int attempts = (_iface == INTERFACE_SPI) ? 3 : 1;
  for (int attempt = 1; attempt <= attempts; attempt++) {
	if (_iface == INTERFACE_SPI) {
	  // wakeup frame (status read); its result is ignored
	  uint8_t w[2] = {PN532_SPI_STATREAD, 0x00};
	  if (!spiTransfer(w, sizeof(w)))
	  {
		LOG("PN532: SPI transfer failed\n");
		return -1;
	  }
	  Sleep(attempt == 1 ? 5 : 50);
	}

	// need to config SAM to stay in Normal Mode
	if (SAMConfig())
	{
	  if (attempt > 1)
		LOG("PN532: SAMConfig succeeded on attempt %d (chip was still waking up)\n", attempt);
	  return 0;
	}
	if (attempt < attempts)
	  LOG("PN532: SAMConfig attempt %d failed, retrying wakeup\n", attempt);
  }
  LOG("PN532: failed to configure SAM (Secure Access Module)\n");
  return -1;
}

/**************************************************************************/
/*!
	@brief  Checks the firmware version of the PN5xx chip

	@returns  The chip's firmware version and ID
*/
/**************************************************************************/
uint32_t PN532::getFirmwareVersion(void) {
  uint32_t response;

  pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

  if (!sendCommandCheckAck(pn532_packetbuffer, 1)) {
	return 0;
  }

  // read data packet
  if (!readdata(pn532_packetbuffer, 13) ||
      !checkResponseFrame(pn532_packetbuffer, 13, PN532_COMMAND_GETFIRMWAREVERSION))
    return 0;

  // check some basic stuff
  if (0 != memcmp((char *)pn532_packetbuffer,
                  (char *)pn532response_firmwarevers, 6)) {
#ifdef PN532DEBUG
    PN532DEBUGPRINT.println(F("Firmware doesn't match!"));
#endif
    return 0;
  }

  int offset = 7;
  response = pn532_packetbuffer[offset++];
  response <<= 8;
  response |= pn532_packetbuffer[offset++];
  response <<= 8;
  response |= pn532_packetbuffer[offset++];
  response <<= 8;
  response |= pn532_packetbuffer[offset++];
  (void)offset;

  return response;
}

/**************************************************************************/
/*!
    @brief  Sends a command and waits a specified period for the ACK

    @param  cmd       Pointer to the command buffer
    @param  cmdlen    The size of the command in bytes
    @param  timeout   timeout before giving up

    @returns  1 if everything is OK, 0 if timeout occured before an
              ACK was recieved
*/
/**************************************************************************/
// default timeout of one second
bool PN532::sendCommandCheckAck(uint8_t *cmd, uint8_t cmdlen,
                                         uint16_t timeout) {

  // I2C works without using IRQ pin by polling for RDY byte
  // seems to work best with some delays between transactions
  uint8_t SLOWDOWN = 0;
  (void)SLOWDOWN;
  //if (i2c_dev || spi_dev) // SPI and I2C need 1ms slow for page reads
    SLOWDOWN = 1;

  // write the command
  if (!writecommand(cmd, cmdlen)) {
    LOG("PN532: failed to write command 0x%02X\n", cmdlen ? cmd[0] : 0);
    return false;
  }

  // I2C TUNING
  Sleep(SLOWDOWN);

  // Wait for chip to say its ready!
  if (!waitready(timeout)) {
    abortCommand();
    return false;
  }

#ifdef PN532DEBUG
  if (spi_dev == NULL) {
    PN532DEBUGPRINT.println(F("IRQ received"));
  }
#endif

  // read acknowledgement
  if (!readack()) {
#ifdef PN532DEBUG
    PN532DEBUGPRINT.println(F("No ACK frame received!"));
#endif
    return false;
  }

  // I2C TUNING
  Sleep(SLOWDOWN);

  // Wait for chip to say its ready!
  if (!waitready(timeout)) {
    // Typical case: InListPassiveTarget with no card in the field. The chip is
    // still executing the command; without an abort it stays busy and every
    // following command fails (on I2C it does not even ACK its address).
    abortCommand();
    return false;
  }

  return true; // ack'd command
}

/**************************************************************************/
/*!
    @brief   Writes an 8-bit value that sets the state of the PN532's GPIO
             pins.
    @param   pinstate  P3 pins state.

    @warning This function is provided exclusively for board testing and
             is dangerous since it will throw an error if any pin other
             than the ones marked "Can be used as GPIO" are modified!  All
             pins that can not be used as GPIO should ALWAYS be left high
             (value = 1) or the system will become unstable and a HW reset
             will be required to recover the PN532.

             pinState[0]  = P30     Can be used as GPIO
             pinState[1]  = P31     Can be used as GPIO
             pinState[2]  = P32     *** RESERVED (Must be 1!) ***
             pinState[3]  = P33     Can be used as GPIO
             pinState[4]  = P34     *** RESERVED (Must be 1!) ***
             pinState[5]  = P35     Can be used as GPIO

    @return  1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool PN532::writeGPIO(uint8_t pinstate) {
  // uint8_t errorbit;

  // Make sure pinstate does not try to toggle P32 or P34
  pinstate |= (1 << PN532_GPIO_P32) | (1 << PN532_GPIO_P34);

  // Fill command buffer
  pn532_packetbuffer[0] = PN532_COMMAND_WRITEGPIO;
  pn532_packetbuffer[1] = PN532_GPIO_VALIDATIONBIT | pinstate; // P3 Pins
  pn532_packetbuffer[2] = 0x00; // P7 GPIO Pins (not used ... taken by SPI)

#ifdef PN532DEBUG
  PN532DEBUGPRINT.print(F("Writing P3 GPIO: "));
  PN532DEBUGPRINT.println(pn532_packetbuffer[1], HEX);
#endif

  // Send the WRITEGPIO command (0x0E)
  if (!sendCommandCheckAck(pn532_packetbuffer, 3))
    return 0x0;

  // Read response packet (00 FF PLEN PLENCHECKSUM D5 CMD+1(0x0F) DATACHECKSUM
  // 00)
  readdata(pn532_packetbuffer, 8);

#ifdef PN532DEBUG
  PN532DEBUGPRINT.print(F("Received: "));
  PrintHex(pn532_packetbuffer, 8);
  PN532DEBUGPRINT.println();
#endif

  int offset = 6;
  return (pn532_packetbuffer[offset] == 0x0F);
}

/**************************************************************************/
/*!
    Reads the state of the PN532's GPIO pins

    @returns An 8-bit value containing the pin state where:

             pinState[0]  = P30
             pinState[1]  = P31
             pinState[2]  = P32
             pinState[3]  = P33
             pinState[4]  = P34
             pinState[5]  = P35
*/
/**************************************************************************/
uint8_t PN532::readGPIO(void) {
  pn532_packetbuffer[0] = PN532_COMMAND_READGPIO;

  // Send the READGPIO command (0x0C)
  if (!sendCommandCheckAck(pn532_packetbuffer, 1))
    return 0x0;

  // Read response packet (00 FF PLEN PLENCHECKSUM D5 CMD+1(0x0D) P3 P7 IO1
  // DATACHECKSUM 00)
  readdata(pn532_packetbuffer, 11);

  /* READGPIO response should be in the following format:

    byte            Description
    -------------   ------------------------------------------
    b0..5           Frame header and preamble (with I2C there is an extra 0x00)
    b6              P3 GPIO Pins
    b7              P7 GPIO Pins (not used ... taken by SPI)
    b8              Interface Mode Pins (not used ... bus select pins)
    b9..10          checksum */

  int p3offset = 7;

#ifdef PN532DEBUG
  PN532DEBUGPRINT.print(F("Received: "));
  PrintHex(pn532_packetbuffer, 11);
  PN532DEBUGPRINT.println();
  PN532DEBUGPRINT.print(F("P3 GPIO: 0x"));
  PN532DEBUGPRINT.println(pn532_packetbuffer[p3offset], HEX);
  PN532DEBUGPRINT.print(F("P7 GPIO: 0x"));
  PN532DEBUGPRINT.println(pn532_packetbuffer[p3offset + 1], HEX);
  PN532DEBUGPRINT.print(F("IO GPIO: 0x"));
  PN532DEBUGPRINT.println(pn532_packetbuffer[p3offset + 2], HEX);
  // Note: You can use the IO GPIO value to detect the serial bus being used
  switch (pn532_packetbuffer[p3offset + 2]) {
  case 0x00: // Using UART
    PN532DEBUGPRINT.println(F("Using UART (IO = 0x00)"));
    break;
  case 0x01: // Using I2C
    PN532DEBUGPRINT.println(F("Using I2C (IO = 0x01)"));
    break;
  case 0x02: // Using SPI
    PN532DEBUGPRINT.println(F("Using SPI (IO = 0x02)"));
    break;
  }
#endif

  return pn532_packetbuffer[p3offset];
}

/**************************************************************************/
/*!
	@brief   Configures the SAM (Secure Access Module)
    @return  true on success, false otherwise.
*/
/**************************************************************************/
bool PN532::SAMConfig(void) {
  pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
  pn532_packetbuffer[1] = 0x01; // normal mode;
  pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
  pn532_packetbuffer[3] = 0x01; // use IRQ pin!

  if (!sendCommandCheckAck(pn532_packetbuffer, 4))
    return false;

  // read data packet
  if (!readdata(pn532_packetbuffer, 9) ||
      !checkResponseFrame(pn532_packetbuffer, 9, PN532_COMMAND_SAMCONFIGURATION))
    return false;

  int offset = 6;
  return (pn532_packetbuffer[offset] == 0x15);
}

/**************************************************************************/
/*!
    Sets the MxRtyPassiveActivation byte of the RFConfiguration register

    @param  maxRetries    0xFF to wait forever, 0x00..0xFE to timeout
                          after mxRetries

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool PN532::setPassiveActivationRetries(uint8_t maxRetries) {
  pn532_packetbuffer[0] = PN532_COMMAND_RFCONFIGURATION;
  pn532_packetbuffer[1] = 5;    // Config item 5 (MaxRetries)
  pn532_packetbuffer[2] = 0xFF; // MxRtyATR (default = 0xFF)
  pn532_packetbuffer[3] = 0x01; // MxRtyPSL (default = 0x01)
  pn532_packetbuffer[4] = maxRetries;

#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Setting MxRtyPassiveActivation to "));
  PN532DEBUGPRINT.print(maxRetries, DEC);
  PN532DEBUGPRINT.println(F(" "));
#endif

  if (!sendCommandCheckAck(pn532_packetbuffer, 5))
    return 0x0; // no ACK

  return 1;
}

/***** ISO14443A Commands ******/

/**************************************************************************/
/*!
    @brief   Waits for an ISO14443A target to enter the field and reads
             its ID.

    @param   cardbaudrate  Baud rate of the card
    @param   uid           Pointer to the array that will be populated
                           with the card's UID (up to 7 bytes)
    @param   uidLength     Pointer to the variable that will hold the
                           length of the card's UID.
    @param   timeout       Timeout in milliseconds.

    @return  1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool PN532::readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid,
                                         uint8_t *uidLength, uint16_t timeout) {
  pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
  pn532_packetbuffer[1] = 1; // max 1 cards at once (we can set this to 2 later)
  pn532_packetbuffer[2] = cardbaudrate;

  if (!sendCommandCheckAck(pn532_packetbuffer, 3, timeout)) {
#ifdef PN532DEBUG
    PN532DEBUGPRINT.println(F("No card(s) read"));
#endif
    return 0x0; // no cards read
  }

  return readDetectedPassiveTargetID(uid, uidLength);
}

/**************************************************************************/
/*!
    @brief   Put the reader in detection mode, non blocking so interrupts
             must be enabled.
    @param   cardbaudrate  Baud rate of the card
    @return  1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool PN532::startPassiveTargetIDDetection(uint8_t cardbaudrate) {
  pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
  pn532_packetbuffer[1] = 1; // max 1 cards at once (we can set this to 2 later)
  pn532_packetbuffer[2] = cardbaudrate;

  return sendCommandCheckAck(pn532_packetbuffer, 3);
}

/**************************************************************************/
/*!
    Reads the ID of the passive target the reader has deteceted.

    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool PN532::readDetectedPassiveTargetID(uint8_t *uid,
                                                 uint8_t *uidLength) {
  // read data packet: 7 header/preamble bytes + NbTg, Tg, SENS_RES(2),
  // SEL_RES, NFCIDLength + up to 10 NFCID bytes (triple size UID) + DCS, postamble
  enum { RESPONSE_LEN = 7 + 6 + 10 + 2 };
  if (!readdata(pn532_packetbuffer, RESPONSE_LEN) ||
      !checkResponseFrame(pn532_packetbuffer, RESPONSE_LEN, PN532_COMMAND_INLISTPASSIVETARGET))
    return 0;
  // check some basic stuff

  /* ISO14443A card response should be in the following format:

    byte            Description
    -------------   ------------------------------------------
    b0..6           Frame header and preamble
    b7              Tags Found
    b8              Tag Number (only one used in this example)
    b9..10          SENS_RES
    b11             SEL_RES
    b12             NFCID Length
    b13..NFCIDLen   NFCID                                      */

#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Found "));
  PN532DEBUGPRINT.print(pn532_packetbuffer[7], DEC);
  PN532DEBUGPRINT.println(F(" tags"));
#endif
  if (pn532_packetbuffer[7] != 1)
    return 0;

  uint16_t sens_res = pn532_packetbuffer[9];
  sens_res <<= 8;
  sens_res |= pn532_packetbuffer[10];
  _lastAtqa = sens_res;
  _lastSak = pn532_packetbuffer[11];
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("ATQA: 0x"));
  PN532DEBUGPRINT.println(sens_res, HEX);
  PN532DEBUGPRINT.print(F("SAK: 0x"));
  PN532DEBUGPRINT.println(pn532_packetbuffer[11], HEX);
#endif

  if (pn532_packetbuffer[12] > PN532_UID_MAX_LEN) {
    LOG("PN532: unexpected UID length %u\n", static_cast<unsigned int>(pn532_packetbuffer[12]));
    return 0;
  }

  /* Card appears to be Mifare Classic */
  *uidLength = pn532_packetbuffer[12];
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("UID:"));
#endif
  for (uint8_t i = 0; i < pn532_packetbuffer[12]; i++) {
    uid[i] = pn532_packetbuffer[13 + i];
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.print(F(" 0x"));
    PN532DEBUGPRINT.print(uid[i], HEX);
#endif
  }
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.println();
#endif

  return 1;
}

/**************************************************************************/
/*!
    @brief   Exchanges an APDU with the currently inlisted peer

    @param   send            Pointer to data to send
    @param   sendLength      Length of the data to send
    @param   response        Pointer to response data
    @param   responseLength  Pointer to the response data length
    @return  true on success, false otherwise.
*/
/**************************************************************************/
bool PN532::inDataExchange(uint8_t *send, uint8_t sendLength,
                                    uint8_t *response,
                                    uint8_t *responseLength) {
  if (sendLength > PN532_PACKBUFFSIZ - 2) {
#ifdef PN532DEBUG
    PN532DEBUGPRINT.println(F("APDU length too long for packet buffer"));
#endif
    return false;
  }
  uint8_t i;

  pn532_packetbuffer[0] = 0x40; // PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = _inListedTag;
  for (i = 0; i < sendLength; ++i) {
    pn532_packetbuffer[i + 2] = send[i];
  }

  if (!sendCommandCheckAck(pn532_packetbuffer, sendLength + 2, 1000)) {
#ifdef PN532DEBUG
    PN532DEBUGPRINT.println(F("Could not send APDU"));
#endif
    return false;
  }

  if (!waitready(1000)) {
#ifdef PN532DEBUG
    PN532DEBUGPRINT.println(F("Response never received for APDU..."));
#endif
    return false;
  }

  readdata(pn532_packetbuffer, sizeof(pn532_packetbuffer));

  if (pn532_packetbuffer[0] == 0 && pn532_packetbuffer[1] == 0 &&
      pn532_packetbuffer[2] == 0xff) {
    uint8_t length = pn532_packetbuffer[3];
    if (pn532_packetbuffer[4] != (uint8_t)(~length + 1)) {
#ifdef PN532DEBUG
      PN532DEBUGPRINT.println(F("Length check invalid"));
      PN532DEBUGPRINT.println(length, HEX);
      PN532DEBUGPRINT.println((~length) + 1, HEX);
#endif
      return false;
    }
    if (pn532_packetbuffer[5] == PN532_PN532TOHOST &&
        pn532_packetbuffer[6] == PN532_RESPONSE_INDATAEXCHANGE) {
      if ((pn532_packetbuffer[7] & 0x3f) != 0) {
#ifdef PN532DEBUG
        PN532DEBUGPRINT.println(F("Status code indicates an error"));
#endif
        return false;
      }

      length -= 3;

      if (length > *responseLength) {
        length = *responseLength; // silent truncation...
      }

      for (i = 0; i < length; ++i) {
        response[i] = pn532_packetbuffer[8 + i];
      }
      *responseLength = length;

      return true;
    } else {
	  LOG("Don't know how to handle command: 0x%02X\n", pn532_packetbuffer[6]);
      return false;
    }
  } else {
    LOG("Preamble missing\n");
    return false;
  }
}

/**************************************************************************/
/*!
    @brief   'InLists' a passive target. PN532 acting as reader/initiator,
             peer acting as card/responder.
    @return  true on success, false otherwise.
*/
/**************************************************************************/
bool PN532::inListPassiveTarget() {
  pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
  pn532_packetbuffer[1] = 1;
  pn532_packetbuffer[2] = 0;

#ifdef PN532DEBUG
  PN532DEBUGPRINT.print(F("About to inList passive target"));
#endif

  if (!sendCommandCheckAck(pn532_packetbuffer, 3, 1000)) {
#ifdef PN532DEBUG
    PN532DEBUGPRINT.println(F("Could not send inlist message"));
#endif
    return false;
  }

  if (!waitready(30000)) {
    return false;
  }

  readdata(pn532_packetbuffer, sizeof(pn532_packetbuffer));

  if (pn532_packetbuffer[0] == 0 && pn532_packetbuffer[1] == 0 &&
      pn532_packetbuffer[2] == 0xff) {
    uint8_t length = pn532_packetbuffer[3];
    if (pn532_packetbuffer[4] != (uint8_t)(~length + 1)) {
#ifdef PN532DEBUG
      PN532DEBUGPRINT.println(F("Length check invalid"));
      PN532DEBUGPRINT.println(length, HEX);
      PN532DEBUGPRINT.println((~length) + 1, HEX);
#endif
      return false;
    }
    if (pn532_packetbuffer[5] == PN532_PN532TOHOST &&
        pn532_packetbuffer[6] == PN532_RESPONSE_INLISTPASSIVETARGET) {
      if (pn532_packetbuffer[7] != 1) {
#ifdef PN532DEBUG
        PN532DEBUGPRINT.println(F("Unhandled number of targets inlisted"));
#endif
		LOG("Number of tags inlisted: %d\n", pn532_packetbuffer[7]);
        return false;
      }

      _inListedTag = pn532_packetbuffer[8];
	  LOG("Tag number: %d\n", _inListedTag);

      return true;
    } else {
#ifdef PN532DEBUG
      PN532DEBUGPRINT.print(F("Unexpected response to inlist passive host"));
#endif
      return false;
    }
  } else {
#ifdef PN532DEBUG
    PN532DEBUGPRINT.println(F("Preamble missing"));
#endif
    return false;
  }
}

/***** Mifare Classic Functions ******/

/**************************************************************************/
/*!
    @brief   Indicates whether the specified block number is the first block
             in the sector (block 0 relative to the current sector)
    @param   uiBlock  Block number to test.
    @return  true if first block, false otherwise.
*/
/**************************************************************************/
bool PN532::mifareclassic_IsFirstBlock(uint32_t uiBlock) {
  // Test if we are in the small or big sectors
  if (uiBlock < 128)
    return ((uiBlock) % 4 == 0);
  else
    return ((uiBlock) % 16 == 0);
}

/**************************************************************************/
/*!
    @brief   Indicates whether the specified block number is the sector
             trailer.
    @param   uiBlock  Block number to test.
    @return  true if sector trailer, false otherwise.
*/
/**************************************************************************/
bool PN532::mifareclassic_IsTrailerBlock(uint32_t uiBlock) {
  // Test if we are in the small or big sectors
  if (uiBlock < 128)
    return ((uiBlock + 1) % 4 == 0);
  else
    return ((uiBlock + 1) % 16 == 0);
}

/**************************************************************************/
/*!
    Tries to authenticate a block of memory on a MIFARE card using the
    INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
    for more information on sending MIFARE and other commands.

    @param  uid           Pointer to a byte array containing the card UID
    @param  uidLen        The length (in bytes) of the card's UID (Should
                          be 4 for MIFARE Classic)
    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  keyNumber     Which key type to use during authentication
                          (0 = MIFARE_CMD_AUTH_A, 1 = MIFARE_CMD_AUTH_B)
    @param  keyData       Pointer to a byte array containing the 6 byte
                          key value

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t PN532::mifareclassic_AuthenticateBlock(uint8_t *uid,
                                                        uint8_t uidLen,
                                                        uint32_t blockNumber,
                                                        uint8_t keyNumber,
                                                        uint8_t *keyData) {
  // uint8_t len;
  uint8_t i;

  // Hang on to the key and uid data
  memcpy(_key, keyData, 6);
  memcpy(_uid, uid, uidLen);
  _uidLen = uidLen;

#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Trying to authenticate card "));
  Adafruit_PN532::PrintHex(_uid, _uidLen);
  PN532DEBUGPRINT.print(F("Using authentication KEY "));
  PN532DEBUGPRINT.print(keyNumber ? 'B' : 'A');
  PN532DEBUGPRINT.print(F(": "));
  Adafruit_PN532::PrintHex(_key, 6);
#endif

  // Prepare the authentication command //
  pn532_packetbuffer[0] =
      PN532_COMMAND_INDATAEXCHANGE; /* Data Exchange Header */
  pn532_packetbuffer[1] = 1;        /* Max card numbers */
  pn532_packetbuffer[2] = (keyNumber) ? MIFARE_CMD_AUTH_B : MIFARE_CMD_AUTH_A;
  pn532_packetbuffer[3] =
      blockNumber; /* Block Number (1K = 0..63, 4K = 0..255 */
  memcpy(pn532_packetbuffer + 4, _key, 6);
  for (i = 0; i < _uidLen; i++) {
    pn532_packetbuffer[10 + i] = _uid[i]; /* 4 byte card ID */
  }

  if (!sendCommandCheckAck(pn532_packetbuffer, 10 + _uidLen))
    return 0;

  // Read the response packet
  readdata(pn532_packetbuffer, 12);

  // check if the response is valid and we are authenticated???
  // for an auth success it should be bytes 5-7: 0xD5 0x41 0x00
  // Mifare auth error is technically byte 7: 0x14 but anything other and 0x00
  // is not good
  if (pn532_packetbuffer[7] != 0x00) {
#ifdef PN532DEBUG
    PN532DEBUGPRINT.print(F("Authentification failed: "));
    Adafruit_PN532::PrintHexChar(pn532_packetbuffer, 12);
#endif
    return 0;
  }

  return 1;
}

/**************************************************************************/
/*!
    Tries to read an entire 16-byte data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          Pointer to the byte array that will hold the
                          retrieved data (if any)

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t PN532::mifareclassic_ReadDataBlock(uint8_t blockNumber,
                                                    uint8_t *data) {
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Trying to read 16 bytes from block "));
  PN532DEBUGPRINT.println(blockNumber);
#endif

  /* Prepare the command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;               /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_READ; /* Mifare Read command = 0x30 */
  pn532_packetbuffer[3] =
      blockNumber; /* Block Number (0..63 for 1K, 0..255 for 4K) */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 4)) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Failed to receive ACK for read command"));
#endif
    return 0;
  }

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);

  /* If byte 8 isn't 0x00 we probably have an error */
  if (pn532_packetbuffer[7] != 0x00) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Unexpected response"));
    Adafruit_PN532::PrintHexChar(pn532_packetbuffer, 26);
#endif
    return 0;
  }

  /* Copy the 16 data bytes to the output buffer        */
  /* Block content starts at byte 9 of a valid response */
  memcpy(data, pn532_packetbuffer + 8, 16);

/* Display data for debug if requested */
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Block "));
  PN532DEBUGPRINT.println(blockNumber);
  Adafruit_PN532::PrintHexChar(data, 16);
#endif

  return 1;
}

/**************************************************************************/
/*!
    Tries to write an entire 16-byte data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          The byte array that contains the data to write.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t PN532::mifareclassic_WriteDataBlock(uint8_t blockNumber,
                                                     uint8_t *data) {
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Trying to write 16 bytes to block "));
  PN532DEBUGPRINT.println(blockNumber);
#endif

  /* Prepare the first command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;                /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_WRITE; /* Mifare Write command = 0xA0 */
  pn532_packetbuffer[3] =
      blockNumber; /* Block Number (0..63 for 1K, 0..255 for 4K) */
  memcpy(pn532_packetbuffer + 4, data, 16); /* Data Payload */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 20)) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Failed to receive ACK for write command"));
#endif
    return 0;
  }
  Sleep(10);

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);

  return 1;
}

/**************************************************************************/
/*!
    Formats a Mifare Classic card to store NDEF Records

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t PN532::mifareclassic_FormatNDEF(void) {
  uint8_t sectorbuffer1[16] = {0x14, 0x01, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1,
                               0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
  uint8_t sectorbuffer2[16] = {0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1,
                               0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
  uint8_t sectorbuffer3[16] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x78, 0x77,
                               0x88, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  // Note 0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 must be used for key A
  // for the MAD sector in NDEF records (sector 0)

  // Write block 1 and 2 to the card
  if (!(mifareclassic_WriteDataBlock(1, sectorbuffer1)))
    return 0;
  if (!(mifareclassic_WriteDataBlock(2, sectorbuffer2)))
    return 0;
  // Write key A and access rights card
  if (!(mifareclassic_WriteDataBlock(3, sectorbuffer3)))
    return 0;

  // Seems that everything was OK (?!)
  return 1;
}

/**************************************************************************/
/*!
    Writes an NDEF URI Record to the specified sector (1..15)

    Note that this function assumes that the Mifare Classic card is
    already formatted to work as an "NFC Forum Tag" and uses a MAD1
    file system.  You can use the NXP TagWriter app on Android to
    properly format cards for this.

    @param  sectorNumber  The sector that the URI record should be written
                          to (can be 1..15 for a 1K card)
    @param  uriIdentifier The uri identifier code (0 = none, 0x01 =
                          "http://www.", etc.)
    @param  url           The uri text to write (max 38 characters).

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t PN532::mifareclassic_WriteNDEFURI(uint8_t sectorNumber,
                                                   uint8_t uriIdentifier,
                                                   const char *url) {
  // Figure out how long the string is
  uint8_t len = strlen(url);

  // Make sure we're within a 1K limit for the sector number
  if ((sectorNumber < 1) || (sectorNumber > 15))
    return 0;

  // Make sure the URI payload is between 1 and 38 chars
  if ((len < 1) || (len > 38))
    return 0;

  // Note 0xD3 0xF7 0xD3 0xF7 0xD3 0xF7 must be used for key A
  // in NDEF records

  // Setup the sector buffer (w/pre-formatted TLV wrapper and NDEF message)
  uint8_t sectorbuffer1[16] = {0x00,
                               0x00,
                               0x03,
                               (uint8_t)(len + 5),
                               0xD1,
                               0x01,
                               (uint8_t)(len + 1),
                               0x55,
                               uriIdentifier,
                               0x00,
                               0x00,
                               0x00,
                               0x00,
                               0x00,
                               0x00,
                               0x00};
  uint8_t sectorbuffer2[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer3[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sectorbuffer4[16] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7, 0x7F, 0x07,
                               0x88, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (len <= 6) {
    // Unlikely we'll get a url this short, but why not ...
    memcpy(sectorbuffer1 + 9, url, len);
    sectorbuffer1[len + 9] = 0xFE;
  } else if (len == 7) {
    // 0xFE needs to be wrapped around to next block
    memcpy(sectorbuffer1 + 9, url, len);
    sectorbuffer2[0] = 0xFE;
  } else if ((len > 7) && (len <= 22)) {
    // Url fits in two blocks
    memcpy(sectorbuffer1 + 9, url, 7);
    memcpy(sectorbuffer2, url + 7, len - 7);
    sectorbuffer2[len - 7] = 0xFE;
  } else if (len == 23) {
    // 0xFE needs to be wrapped around to final block
    memcpy(sectorbuffer1 + 9, url, 7);
    memcpy(sectorbuffer2, url + 7, len - 7);
    sectorbuffer3[0] = 0xFE;
  } else {
    // Url fits in three blocks
    memcpy(sectorbuffer1 + 9, url, 7);
    memcpy(sectorbuffer2, url + 7, 16);
    memcpy(sectorbuffer3, url + 23, len - 24);
    sectorbuffer3[len - 22] = 0xFE;
  }

  // Now write all three blocks back to the card
  if (!(mifareclassic_WriteDataBlock(sectorNumber * 4, sectorbuffer1)))
    return 0;
  if (!(mifareclassic_WriteDataBlock((sectorNumber * 4) + 1, sectorbuffer2)))
    return 0;
  if (!(mifareclassic_WriteDataBlock((sectorNumber * 4) + 2, sectorbuffer3)))
    return 0;
  if (!(mifareclassic_WriteDataBlock((sectorNumber * 4) + 3, sectorbuffer4)))
    return 0;

  // Seems that everything was OK (?!)
  return 1;
}

/***** Mifare Ultralight Functions ******/

/**************************************************************************/
/*!
    @brief   Tries to read an entire 4-byte page at the specified address.

    @param   page        The page number (0..63 in most cases)
    @param   buffer      Pointer to the byte array that will hold the
                         retrieved data (if any)
    @return  1 on success, 0 on error.
*/
/**************************************************************************/
uint8_t PN532::mifareultralight_ReadPage(uint8_t page,
                                                  uint8_t *buffer) {
  if (page >= 64) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Page value out of range"));
#endif
    return 0;
  }

#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Reading page "));
  PN532DEBUGPRINT.println(page);
#endif

  /* Prepare the command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;               /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_READ; /* Mifare Read command = 0x30 */
  pn532_packetbuffer[3] = page; /* Page Number (0..63 in most cases) */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 4)) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Failed to receive ACK for write command"));
#endif
    return 0;
  }

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.println(F("Received: "));
  Adafruit_PN532::PrintHexChar(pn532_packetbuffer, 26);
#endif

  /* If byte 8 isn't 0x00 we probably have an error */
  if (pn532_packetbuffer[7] == 0x00) {
    /* Copy the 4 data bytes to the output buffer         */
    /* Block content starts at byte 9 of a valid response */
    /* Note that the command actually reads 16 byte or 4  */
    /* pages at a time ... we simply discard the last 12  */
    /* bytes                                              */
    memcpy(buffer, pn532_packetbuffer + 8, 4);
  } else {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Unexpected response reading block: "));
    Adafruit_PN532::PrintHexChar(pn532_packetbuffer, 26);
#endif
    return 0;
  }

/* Display data for debug if requested */
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Page "));
  PN532DEBUGPRINT.print(page);
  PN532DEBUGPRINT.println(F(":"));
  Adafruit_PN532::PrintHexChar(buffer, 4);
#endif

  // Return OK signal
  return 1;
}

/**************************************************************************/
/*!
    Tries to write an entire 4-byte page at the specified block
    address.

    @param  page          The page number to write.  (0..63 for most cases)
    @param  data          The byte array that contains the data to write.
                          Should be exactly 4 bytes long.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t PN532::mifareultralight_WritePage(uint8_t page,
                                                   uint8_t *data) {

  if (page >= 64) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Page value out of range"));
#endif
    // Return Failed Signal
    return 0;
  }

#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Trying to write 4 byte page"));
  PN532DEBUGPRINT.println(page);
#endif

  /* Prepare the first command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1; /* Card number */
  pn532_packetbuffer[2] =
      MIFARE_ULTRALIGHT_CMD_WRITE; /* Mifare Ultralight Write command = 0xA2 */
  pn532_packetbuffer[3] = page;    /* Page Number (0..63 for most cases) */
  memcpy(pn532_packetbuffer + 4, data, 4); /* Data Payload */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 8)) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Failed to receive ACK for write command"));
#endif

    // Return Failed Signal
    return 0;
  }
  Sleep(10);

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);

  // Return OK Signal
  return 1;
}

/***** NTAG2xx Functions ******/

/**************************************************************************/
/*!
    @brief   Tries to read an entire 4-byte page at the specified address.

    @param   page        The page number (0..63 in most cases)
    @param   buffer      Pointer to the byte array that will hold the
                         retrieved data (if any)
    @return  1 on success, 0 on error.
*/
/**************************************************************************/
uint8_t PN532::ntag2xx_ReadPage(uint8_t page, uint8_t *buffer) {
  // TAG Type       PAGES   USER START    USER STOP
  // --------       -----   ----------    ---------
  // NTAG 203       42      4             39
  // NTAG 213       45      4             39
  // NTAG 215       135     4             129
  // NTAG 216       231     4             225

  if (page >= 231) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Page value out of range"));
#endif
    return 0;
  }

#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Reading page "));
  PN532DEBUGPRINT.println(page);
#endif

  /* Prepare the command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;               /* Card number */
  pn532_packetbuffer[2] = MIFARE_CMD_READ; /* Mifare Read command = 0x30 */
  pn532_packetbuffer[3] = page; /* Page Number (0..63 in most cases) */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 4)) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Failed to receive ACK for write command"));
#endif
    return 0;
  }

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.println(F("Received: "));
  Adafruit_PN532::PrintHexChar(pn532_packetbuffer, 26);
#endif

  /* If byte 8 isn't 0x00 we probably have an error */
  if (pn532_packetbuffer[7] == 0x00) {
    /* Copy the 4 data bytes to the output buffer         */
    /* Block content starts at byte 9 of a valid response */
    /* Note that the command actually reads 16 byte or 4  */
    /* pages at a time ... we simply discard the last 12  */
    /* bytes                                              */
    memcpy(buffer, pn532_packetbuffer + 8, 4);
  } else {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Unexpected response reading block: "));
    Adafruit_PN532::PrintHexChar(pn532_packetbuffer, 26);
#endif
    return 0;
  }

/* Display data for debug if requested */
#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Page "));
  PN532DEBUGPRINT.print(page);
  PN532DEBUGPRINT.println(F(":"));
  Adafruit_PN532::PrintHexChar(buffer, 4);
#endif

  // Return OK signal
  return 1;
}

/**************************************************************************/
/*!
    Tries to write an entire 4-byte page at the specified block
    address.

    @param  page          The page number to write.  (0..63 for most cases)
    @param  data          The byte array that contains the data to write.
                          Should be exactly 4 bytes long.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t PN532::ntag2xx_WritePage(uint8_t page, uint8_t *data) {
  // TAG Type       PAGES   USER START    USER STOP
  // --------       -----   ----------    ---------
  // NTAG 203       42      4             39
  // NTAG 213       45      4             39
  // NTAG 215       135     4             129
  // NTAG 216       231     4             225

  if ((page < 4) || (page > 225)) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Page value out of range"));
#endif
    // Return Failed Signal
    return 0;
  }

#ifdef MIFAREDEBUG
  PN532DEBUGPRINT.print(F("Trying to write 4 byte page"));
  PN532DEBUGPRINT.println(page);
#endif

  /* Prepare the first command */
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1; /* Card number */
  pn532_packetbuffer[2] =
      MIFARE_ULTRALIGHT_CMD_WRITE; /* Mifare Ultralight Write command = 0xA2 */
  pn532_packetbuffer[3] = page;    /* Page Number (0..63 for most cases) */
  memcpy(pn532_packetbuffer + 4, data, 4); /* Data Payload */

  /* Send the command */
  if (!sendCommandCheckAck(pn532_packetbuffer, 8)) {
#ifdef MIFAREDEBUG
    PN532DEBUGPRINT.println(F("Failed to receive ACK for write command"));
#endif

    // Return Failed Signal
    return 0;
  }
  Sleep(10);

  /* Read the response packet */
  readdata(pn532_packetbuffer, 26);

  // Return OK Signal
  return 1;
}

/**************************************************************************/
/*!
    Writes an NDEF URI Record starting at the specified page (4..nn)

    Note that this function assumes that the NTAG2xx card is
    already formatted to work as an "NFC Forum Tag".

    @param  uriIdentifier The uri identifier code (0 = none, 0x01 =
                          "http://www.", etc.)
    @param  url           The uri text to write (null-terminated string).
    @param  dataLen       The size of the data area for overflow checks.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t PN532::ntag2xx_WriteNDEFURI(uint8_t uriIdentifier, char *url,
                                             uint8_t dataLen) {
  uint8_t pageBuffer[4] = {0, 0, 0, 0};

  // Remove NDEF record overhead from the URI data (pageHeader below)
  uint8_t wrapperSize = 12;

  // Figure out how long the string is
  uint8_t len = strlen(url);

  // Make sure the URI payload will fit in dataLen (include 0xFE trailer)
  if ((len < 1) || (len + 1 > (dataLen - wrapperSize)))
    return 0;

  // Setup the record header
  // See NFCForum-TS-Type-2-Tag_1.1.pdf for details
  uint8_t pageHeader[12] = {
      /* NDEF Lock Control TLV (must be first and always present) */
      0x01, /* Tag Field (0x01 = Lock Control TLV) */
      0x03, /* Payload Length (always 3) */
      0xA0, /* The position inside the tag of the lock bytes (upper 4 = page
               address, lower 4 = byte offset) */
      0x10, /* Size in bits of the lock area */
      0x44, /* Size in bytes of a page and the number of bytes each lock bit can
               lock (4 bit + 4 bits) */
      /* NDEF Message TLV - URI Record */
      0x03,               /* Tag Field (0x03 = NDEF Message) */
      (uint8_t)(len + 5), /* Payload Length (not including 0xFE trailer) */
      0xD1, /* NDEF Record Header (TNF=0x1:Well known record + SR + ME + MB) */
      0x01, /* Type Length for the record type indicator */
      (uint8_t)(len + 1), /* Payload len */
      0x55,               /* Record Type Indicator (0x55 or 'U' = URI Record) */
      uriIdentifier       /* URI Prefix (ex. 0x01 = "http://www.") */
  };

  // Write 12 byte header (three pages of data starting at page 4)
  memcpy(pageBuffer, pageHeader, 4);
  if (!(ntag2xx_WritePage(4, pageBuffer)))
    return 0;
  memcpy(pageBuffer, pageHeader + 4, 4);
  if (!(ntag2xx_WritePage(5, pageBuffer)))
    return 0;
  memcpy(pageBuffer, pageHeader + 8, 4);
  if (!(ntag2xx_WritePage(6, pageBuffer)))
    return 0;

  // Write URI (starting at page 7)
  uint8_t currentPage = 7;
  char *urlcopy = url;
  while (len) {
    if (len < 4) {
      memset(pageBuffer, 0, 4);
      memcpy(pageBuffer, urlcopy, len);
      pageBuffer[len] = 0xFE; // NDEF record footer
      if (!(ntag2xx_WritePage(currentPage, pageBuffer)))
        return 0;
      // DONE!
      return 1;
    } else if (len == 4) {
      memcpy(pageBuffer, urlcopy, len);
      if (!(ntag2xx_WritePage(currentPage, pageBuffer)))
        return 0;
      memset(pageBuffer, 0, 4);
      pageBuffer[0] = 0xFE; // NDEF record footer
      currentPage++;
      if (!(ntag2xx_WritePage(currentPage, pageBuffer)))
        return 0;
      // DONE!
      return 1;
    } else {
      // More than one page of data left
      memcpy(pageBuffer, urlcopy, 4);
      if (!(ntag2xx_WritePage(currentPage, pageBuffer)))
        return 0;
      currentPage++;
      urlcopy += 4;
      len -= 4;
    }
  }

  // Seems that everything was OK (?!)
  return 1;
}

/************** high level communication functions (handles both I2C and SPI) */

/**************************************************************************/
/*!
    @brief  Tries to read the SPI or I2C ACK signal
*/
/**************************************************************************/
uint8_t PN532::ntag2xx_GetVersion(uint8_t *version) {
  // Built like ntag2xx_ReadPage (card number hardcoded to 1) rather than via
  // inDataExchange(), which uses _inListedTag - only set by inListPassiveTarget,
  // not by the readPassiveTargetID the tools use.
  pn532_packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
  pn532_packetbuffer[1] = 1;    // card number
  pn532_packetbuffer[2] = 0x60; // GET_VERSION

  if (!sendCommandCheckAck(pn532_packetbuffer, 3))
    return 0;

  readdata(pn532_packetbuffer, 26);
  if (pn532_packetbuffer[7] != 0x00)	// InDataExchange status byte
    return 0;

  memcpy(version, pn532_packetbuffer + 8, 8);
  return 1;
}

const char* PN532::describeNtagFromVersion(const uint8_t *version) {
  // GET_VERSION layout: [0]=fixed 0x00, [1]=vendor, [2]=product type,
  // [3]=subtype, [4]=major, [5]=minor, [6]=storage size code, [7]=protocol.
  uint8_t productType = version[2];
  uint8_t storage = version[6];
  if (productType == 0x04) {			// NTAG
    switch (storage) {
    case 0x0F: return "NTAG213 (144 bytes user memory)";
    case 0x11: return "NTAG215 (504 bytes user memory)";
    case 0x13: return "NTAG216 (888 bytes user memory)";
    default:   return "NTAG21x (unknown storage size)";
    }
  }
  if (productType == 0x03) {			// MIFARE Ultralight EV1
    switch (storage) {
    case 0x0B: return "MIFARE Ultralight EV1 MF0UL11 (48 bytes user memory)";
    case 0x0E: return "MIFARE Ultralight EV1 MF0UL21 (128 bytes user memory)";
    default:   return "MIFARE Ultralight EV1 (unknown storage size)";
    }
  }
  return "unknown Ultralight/NTAG variant";
}

const char* PN532::describeManufacturer(uint8_t manufacturerByte) {
  // ISO/IEC 7816-6 registered application provider identifiers (common subset).
  switch (manufacturerByte) {
  case 0x02: return "STMicroelectronics";
  case 0x04: return "NXP Semiconductors";
  case 0x05: return "Infineon Technologies";
  case 0x07: return "Texas Instruments";
  case 0x08: return "Fujitsu";
  case 0x16: return "EM Microelectronic-Marin";
  case 0x21: return "EM Microelectronic-Marin";
  case 0x28: return "LG-Semiconductors";
  case 0x2B: return "Maxim";
  default:   return "unknown manufacturer";
  }
}

const char* PN532::describeMifareMemory(uint8_t sak) {
  switch (sak) {
  case 0x08: return "1 KB: 16 sectors x 4 blocks (64 blocks x 16 B)";
  case 0x09: return "MIFARE Mini: 320 B, 5 sectors x 4 blocks";
  case 0x18: return "4 KB: 32 sectors x 4 + 8 sectors x 16 blocks (256 blocks)";
  case 0x10: return "MIFARE Plus 2 KB (SL2)";
  case 0x11: return "MIFARE Plus 4 KB (SL2)";
  default:   return "";
  }
}

const char* PN532::describeIso14443aCard(uint16_t atqa, uint8_t sak) {
  // SAK identifies the type in most cases; ATQA separates the few SAK values
  // shared by different families. Clones usually report the original's values.
  switch (sak) {
  case 0x00:
    if (atqa == 0x0044)
      return "MIFARE Ultralight / Ultralight C / NTAG2xx";
    return "MIFARE Ultralight family (unusual ATQA)";
  case 0x08:
    return "MIFARE Classic 1K (or Plus 2K/4K in SL1, or clone)";
  case 0x09:
    return "MIFARE Mini";
  case 0x10:
    return "MIFARE Plus 2K (SL2)";
  case 0x11:
    return "MIFARE Plus 4K (SL2)";
  case 0x18:
    return "MIFARE Classic 4K (or Plus 4K in SL1, or clone)";
  case 0x20:
    if (atqa == 0x0344)
      return "MIFARE DESFire (EV1/EV2/EV3)";
    return "ISO14443-4 card (DESFire, Plus SL3, smart card, phone emulation...)";
  case 0x28:
    return "SmartMX with MIFARE Classic 1K emulation";
  case 0x38:
    return "SmartMX with MIFARE Classic 4K emulation";
  case 0x88:
    return "Infineon MIFARE Classic 1K";
  case 0x98:
    return "Gemplus MPCOS";
  default:
    return "unknown ISO14443A card";
  }
}

/*! @brief  Abort the command the PN532 is currently executing.
    Per the PN532 user manual an ACK frame sent by the host aborts the
    current process; the chip then accepts new commands again.
*/
void PN532::abortCommand(void) {
  LOG("PN532: aborting current command\n");
  if (_iface == INTERFACE_SPI) {
    uint8_t buf[1 + sizeof(pn532ack)];
    buf[0] = PN532_SPI_DATAWRITE;
    memcpy(buf + 1, pn532ack, sizeof(pn532ack));
    spiTransfer(buf, sizeof(buf));
  } else {
    uint8_t buf[sizeof(pn532ack)];
    memcpy(buf, pn532ack, sizeof(pn532ack));
    ch341SoftwareI2C.writeBytesToDevice(buf, sizeof(buf));
  }
  Sleep(2);
}

/*! @brief  Validate a normal information frame read into buff (n bytes):
    preamble/start code, LEN + LCS, TFI, response code (command + 1) and DCS.
    The whole frame (5 header bytes + LEN + DCS) must fit into the n bytes read.
*/
bool PN532::checkResponseFrame(const uint8_t *buff, uint8_t n, uint8_t command) {
  if (n < 8 || buff[0] != PN532_PREAMBLE || buff[1] != PN532_STARTCODE1 ||
      buff[2] != PN532_STARTCODE2) {
    LOG("PN532: response: bad start code\n");
    return false;
  }
  uint8_t len = buff[3];
  if (static_cast<uint8_t>(len + buff[4]) != 0) {
    LOG("PN532: response: bad length checksum\n");
    return false;
  }
  if (len < 2 || 5 + len + 1 > n) {
    LOG("PN532: response: length %u does not fit %u bytes read\n",
      static_cast<unsigned int>(len), static_cast<unsigned int>(n));
    return false;
  }
  if (buff[5] != PN532_PN532TOHOST || buff[6] != static_cast<uint8_t>(command + 1)) {
    LOG("PN532: response: unexpected TFI/code %02X %02X for command %02X\n",
      buff[5], buff[6], command);
    return false;
  }
  uint8_t sum = 0;
  for (unsigned int i = 5; i < 5u + len + 1u; i++)	// TFI..data + DCS
    sum = static_cast<uint8_t>(sum + buff[i]);
  if (sum != 0) {
    LOG("PN532: response: bad data checksum\n");
    return false;
  }
  return true;
}

bool PN532::readack() {
  uint8_t ackbuff[6];

  if (!readdata(ackbuff, 6))
    return false;
  return (0 == memcmp((char *)ackbuff, (char *)pn532ack, 6));
}

/*! @brief  Return true if the PN532 is ready with a response.
*/
bool PN532::isready() {
	if (_iface == INTERFACE_SPI) {
		uint8_t buf[2] = {PN532_SPI_STATREAD, 0x00};
		if (!spiTransfer(buf, sizeof(buf)))
			return false;
		return (buf[1] & PN532_SPI_READY) != 0;
	}
	// I2C ready check via reading RDY byte. While busy, the PN532 simply does
	// not ACK its own I2C address at all, so read1bFromDevice() fails and never
	// writes to rdy - treat that as "not ready" rather than reading garbage.
	uint8_t rdy = 0;
	if (!ch341SoftwareI2C.read1bFromDevice(&rdy))
		return false;
	return rdy == PN532_I2C_READY;
}

/**************************************************************************/
/*!
    @brief  Waits until the PN532 is ready.

    @param  timeout   Timeout before giving up
*/
/**************************************************************************/
bool PN532::waitready(unsigned int timeout) {
  unsigned int startTimer = timeGetTime();
  while (!isready()) {
	if (timeout != 0 && timeGetTime() - startTimer > timeout) {
		LOG("PN532: timeout waiting for ready\n");
		return false;
	}
    Sleep(10);
  }
  return true;
}

/**************************************************************************/
/*!
    @brief  Reads n bytes of data from the PN532 via SPI or I2C.

    @param  buff      Pointer to the buffer where data will be written
    @param  n         Number of bytes to be read
*/
/**************************************************************************/
bool PN532::readdata(uint8_t *buff, uint8_t n) {
	// +1: leading RDY byte (I2C) / DATAREAD opcode slot (SPI)
	std::vector<uint8_t> rbuff(n + 1, 0);
	bool ok;
	if (_iface == INTERFACE_SPI) {
		rbuff[0] = PN532_SPI_DATAREAD;
		ok = spiTransfer(&rbuff[0], rbuff.size());
	} else {
		//ch341a.I2CReadBytes(PN532_I2C_ADDRESS, &rbuff[0], rbuff.size());
		ok = ch341SoftwareI2C.readBytesFromDevice(&rbuff[0], static_cast<uint8_t>(rbuff.size())) == 1;
	}
	if (!ok) {
		// Zeroed rather than partial data: every caller's frame header check
		// then fails cleanly instead of parsing a half-read response
		std::fill(rbuff.begin(), rbuff.end(), 0);
		LOG("PN532: failed to read %u bytes\n", static_cast<unsigned int>(n));
	}
    for (uint8_t i = 0; i < n; i++) {
      buff[i] = rbuff[i + 1];
    }
#ifdef PN532DEBUG
  PN532DEBUGPRINT.print(F("Reading: "));
  for (uint8_t i = 0; i < n; i++) {
    PN532DEBUGPRINT.print(F(" 0x"));
    PN532DEBUGPRINT.print(buff[i], HEX);
  }
  PN532DEBUGPRINT.println();
#endif
  return ok;
}

/**************************************************************************/
/*!
    @brief   set the PN532 as iso14443a Target behaving as a SmartCard
    @return  true on success, false otherwise.
    @note    Author: Salvador Mendoza (salmg.net) new functions:
             -AsTarget
             -getDataTarget
             -setDataTarget
*/
/**************************************************************************/
uint8_t PN532::AsTarget() {
  pn532_packetbuffer[0] = 0x8C;
  uint8_t target[] = {
      0x8C,             // INIT AS TARGET
      0x00,             // MODE -> BITFIELD
      0x08, 0x00,       // SENS_RES - MIFARE PARAMS
      0xdc, 0x44, 0x20, // NFCID1T
      0x60,             // SEL_RES
      0x01, 0xfe, // NFCID2T MUST START WITH 01fe - FELICA PARAMS - POL_RES
      0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xc0,
      0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, // PAD
      0xff, 0xff,                               // SYSTEM CODE
      0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
      0x33, 0x22, 0x11, 0x01, 0x00, // NFCID3t MAX 47 BYTES ATR_RES
      0x0d, 0x52, 0x46, 0x49, 0x44, 0x49, 0x4f,
      0x74, 0x20, 0x50, 0x4e, 0x35, 0x33, 0x32 // HISTORICAL BYTES
  };
  if (!sendCommandCheckAck(target, sizeof(target)))
    return false;

  // read data packet
  readdata(pn532_packetbuffer, 8);

  int offset = 6;
  return (pn532_packetbuffer[offset] == 0x15);
}
/**************************************************************************/
/*!
    @brief   Retrieve response from the emulation mode

    @param   cmd    = data
    @param   cmdlen = data length
    @return  true on success, false otherwise.
*/
/**************************************************************************/
uint8_t PN532::getDataTarget(uint8_t *cmd, uint8_t *cmdlen) {
  uint8_t length;
  pn532_packetbuffer[0] = 0x86;
  if (!sendCommandCheckAck(pn532_packetbuffer, 1, 1000)) {
    LOG("Error en ack\n");
    return false;
  }

  // read data packet
  readdata(pn532_packetbuffer, 64);
  length = pn532_packetbuffer[3] - 3;

  // if (length > *responseLength) {// Bug, should avoid it in the reading
  // target data
  //  length = *responseLength; // silent truncation...
  //}

  for (int i = 0; i < length; ++i) {
    cmd[i] = pn532_packetbuffer[8 + i];
  }
  *cmdlen = length;
  return true;
}

/**************************************************************************/
/*!
    @brief   Set data in PN532 in the emulation mode

    @param   cmd    = data
    @param   cmdlen = data length
    @return  true on success, false otherwise.
*/
/**************************************************************************/
uint8_t PN532::setDataTarget(uint8_t *cmd, uint8_t cmdlen) {
  uint8_t length;
  // cmd1[0] = 0x8E; Must!

  if (!sendCommandCheckAck(cmd, cmdlen))
    return false;

  // read data packet
  readdata(pn532_packetbuffer, 8);
  length = pn532_packetbuffer[3] - 3;
  for (int i = 0; i < length; ++i) {
    cmd[i] = pn532_packetbuffer[8 + i];
  }
  // cmdl = 0
  //cmdlen = length;

  int offset = 6;
  return (pn532_packetbuffer[offset] == 0x15);
}

/*! @brief  Writes a command to the PN532, automatically inserting the
            preamble and required frame details (checksum, len, etc.)
    @param  cmd       Pointer to the command buffer
	@param  cmdlen    Command length in bytes
*/
bool PN532::writecommand(uint8_t *cmd, uint8_t cmdlen) {
	std::vector<uint8_t> packet;
	packet.resize(8 + cmdlen);
    uint8_t LEN = cmdlen + 1;

    packet[0] = PN532_PREAMBLE;
    packet[1] = PN532_STARTCODE1;
    packet[2] = PN532_STARTCODE2;
    packet[3] = LEN;
    packet[4] = ~LEN + 1;
    packet[5] = PN532_HOSTTOPN532;
    uint8_t sum = 0;
    for (uint8_t i = 0; i < cmdlen; i++) {
      packet[6 + i] = cmd[i];
      sum += cmd[i];
    }
    packet[6 + cmdlen] = ~(PN532_HOSTTOPN532 + sum) + 1;
    packet[7 + cmdlen] = PN532_POSTAMBLE;

#ifdef PN532DEBUG
    Serial.print("Sending : ");
    for (int i = 1; i < 8 + cmdlen; i++) {
      Serial.print("0x");
      Serial.print(packet[i], HEX);
      Serial.print(", ");
    }
    Serial.println();
#endif

	if (_iface == INTERFACE_SPI) {
		packet.insert(packet.begin(), PN532_SPI_DATAWRITE);
		return spiTransfer(&packet[0], packet.size());
	} else {
		//ch341a.I2CWriteBytes(PN532_I2C_ADDRESS, &packet[0], packet.size());
		return ch341SoftwareI2C.writeBytesToDevice(&packet[0], static_cast<uint8_t>(packet.size())) == 1;
	}
}
