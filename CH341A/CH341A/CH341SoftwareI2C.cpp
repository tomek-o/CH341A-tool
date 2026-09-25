//---------------------------------------------------------------------------
#pragma hdrstop

/*
	Software (bit-banged) I2C (CH341SoftwareI2C, used e.g. by PN532 over I2C)
	requires RXD connected to SCL:
	- CH341 SCL is output-only (CH341SetOutput bit 18) and cannot be read back;
	  CH341GetInput has no SCL bit (SDA is readable as bit 23).
	- Clock stretching (slave holding SCL low while busy, e.g. PN532) needs the
	  real SCL state, so SCL is wired to a readable pin: input bit 9 = PEMP pin,
	  which should be the pin labeled RXD on the programmer header (ERR# = TXD,
	  PEMP = RXD in UART mode - not verified against the datasheet).
	- Both CH341 SCL and RXD go to the same slave SCL line - it is one net:

	    CH341 SCL --+-- slave SCL
	    CH341 RXD --+     (RXD only reads, never drives)
	    CH341 SDA ------- slave SDA

	- SCL/SDA need pull-ups (typ. 4.7k to 3.3 V; most breakout boards have them).
	  CH341 SCL/SDA are open-drain, so with pull-ups to 3.3 V a 5 V CH341A board
	  does not push 5 V onto the bus.
	- Without the RXD-SCL jumper CH341SoftwareI2C::begin() fails with
	  "SCL input (RXD) reads low ... is RXD connected to SCL?" in the log.
*/

/*
	Heavily based on SWI2C Library
	https://github.com/Andy4495/SWI2C
	MIT License

	Copyright (c) 2018 - 2023 Andreas Taylor

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "CH341SoftwareI2C.h"
#include "CH341A.h"
#include "Log.h"
#include <windows.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

enum
{
    SCL_OUT_PIN_ID = 18,	// SCL, output only, open drain
    SCL_IN_PIN_ID = 9,		// RXD line connected together with SCL, used as input for clock stretching
    SDA_OUT_PIN_ID = 19,	// SDA, semi-bidirectional with open-drain output and pull-up
    SDA_IN_PIN_ID = 23		// SDA, separate bit required for reading input
};

enum
{
    RECOVERY_STRETCH_TIMEOUT = 20UL,	// ms; short, the bus is suspect during recovery anyway
    RECOVERY_CLOCKS = 9					// enough to finish any byte + ACK a slave may be stuck in
};

bool sclState = false;
bool sdaState = false;

/** Set on any failed CH341 GPIO access; a failed read or write means the
	bus state is unknown, so the whole transfer must be treated as failed
	instead of e.g. reading the missing input as an ACK / 0 bit.
*/
bool gpioError = false;

/** Last state actually written to the pins; update() skips writes that would
	not change anything (e.g. releasing an already released SDA), saving USB
	round-trips. Invalidated whenever the pin state may be unknown.
*/
bool outputsValid = false;
bool writtenScl = false;
bool writtenSda = false;

void update(void)
{
    if (outputsValid && !gpioError && writtenScl == sclState && writtenSda == sdaState)
        return;

    uint32_t direction = (1u << SCL_OUT_PIN_ID) | (1u << SDA_OUT_PIN_ID);
    uint32_t value = 0;
    if (sclState)
        value |= (1u << SCL_OUT_PIN_ID);
    if (sdaState)
        value |= (1u << SDA_OUT_PIN_ID);

    if (ch341a.SetGpioOutputs(direction, value) != 0)
    {
        if (!gpioError)
            LOG("SoftwareI2C: setting outputs failed!\n");
        gpioError = true;
        outputsValid = false;
        return;
    }
    writtenScl = sclState;
    writtenSda = sdaState;
    outputsValid = true;
}

/** \return 1 if pin high, 0 if low, -1 on read failure */
int GetInput(unsigned int pinId)
{
    uint32_t dataIn = 0;

    int status = ch341a.GetGpioInputs(dataIn);
    if (status != 0)
    {
        if (!gpioError)
            LOG("SoftwareI2C: reading inputs failed!\n");
        gpioError = true;
        return -1;
    }
    return (dataIn & (1u << pinId)) ? 1 : 0;
}

#define millis timeGetTime

}	// namespace

CH341SoftwareI2C ch341SoftwareI2C;

CH341SoftwareI2C::CH341SoftwareI2C(void)
{
    _deviceID = 0;
    _stretch_timeout_delay = DEFAULT_STRETCH_TIMEOUT;
    _stretch_timeout_error = 0;
}

int CH341SoftwareI2C::begin()
{
    gpioError = false;
    outputsValid = false;	// other tools may have changed the pins meanwhile
    sclState = true;
    sdaState = true;
    update();
    if (gpioError)
        return -1;
    Sleep(1);

    // SCL is only readable through RXD wired to it. Without that jumper every
    // sclHi() would wait for the full stretch timeout and every transfer fail,
    // with nothing pointing at the cause.
    int scl = GetInput(SCL_IN_PIN_ID);
    if (scl < 0)
        return -1;
    if (scl == 0)
    {
        LOG("SoftwareI2C: SCL input (RXD) reads low with SCL released - is RXD connected to SCL? (or SCL shorted / missing pull-up)\n");
        return -2;
    }

    // A transfer aborted mid-byte (timeout, USB error, tool closed) can leave
    // a slave holding SDA low forever; clear that before the first START.
    int sda = GetInput(SDA_IN_PIN_ID);
    if (sda < 0)
        return -1;
    if (sda == 0)
    {
        LOG("SoftwareI2C: SDA held low on begin, trying bus recovery\n");
        if (busRecover() != 0)
            return -3;
    }
    return 0;
}

int CH341SoftwareI2C::busRecover()
{
    unsigned long savedTimeout = _stretch_timeout_delay;
    _stretch_timeout_delay = RECOVERY_STRETCH_TIMEOUT;
    gpioError = false;
    outputsValid = false;

    sdaHi();
    for (int i = 0; i < RECOVERY_CLOCKS; i++)
    {
        if (GetInput(SDA_IN_PIN_ID) == 1)
            break;
        sclLo();
        Sleep(1);
        sclHi();
        Sleep(1);
    }
    // STOP: SDA low -> high while SCL high
    sclLo();
    sdaLo();
    sclHi();
    sdaHi();

    _stretch_timeout_delay = savedTimeout;

    int sda = GetInput(SDA_IN_PIN_ID);
    int scl = GetInput(SCL_IN_PIN_ID);
    if (sda == 1 && scl == 1)
    {
        LOG("SoftwareI2C: bus recovered\n");
        return 0;
    }
    LOG("SoftwareI2C: bus recovery failed (SDA = %d, SCL = %d)\n", sda, scl);
    return -1;
}

bool CH341SoftwareI2C::failed(void) const
{
    return gpioError || _stretch_timeout_error;
}

bool CH341SoftwareI2C::sendByteCheckAck(uint8_t data)
{
    writeByte(data);
    uint8_t nack = checkAckBit();
    return !failed() && nack == 0;
}

int CH341SoftwareI2C::transfer(const uint8_t *regAddress, const uint8_t *wbuf, uint8_t wcount,
    uint8_t *rbuf, uint8_t rcount, bool sendStopBit)
{
    // Common implementation of all high level methods:
    //   [START addr+W [reg] [wbuf...]] [(re)START addr+R rbuf...] [STOP]
    // Returns 1 on success, 0 on NACK or bus error. On a bus error (clock
    // stretch timeout, GPIO failure) the slave may be mid-byte, so the bus is
    // recovered before returning instead of leaving SDA possibly held low.
    gpioError = false;
    _stretch_timeout_error = 0;

    bool writePhase = (regAddress != NULL) || (wcount > 0) || (rcount == 0);

    startBit();
    if (writePhase)
    {
        bool ok = sendByteCheckAck(static_cast<uint8_t>(_deviceID << 1));
        if (ok && regAddress)
            ok = sendByteCheckAck(*regAddress);
        for (uint8_t i = 0; ok && i < wcount; i++)
            ok = sendByteCheckAck(wbuf[i]);
        if (!ok)
            return abortTransfer();
    }

    if (rcount > 0)
    {
        if (writePhase)
            startBit();		// repeated START
        if (!sendByteCheckAck(static_cast<uint8_t>((_deviceID << 1) | 1)))
            return abortTransfer();
        for (uint8_t i = 0; i < rcount; i++)
        {
            rbuf[i] = read1Byte();
            if (i < rcount - 1)
                writeAck();
            else
                checkAckBit();	// released SDA = NACK: controller ends the read
            if (failed())
                return abortTransfer();
        }
    }

    if (sendStopBit)
        stopBit();
    if (failed())
        return abortTransfer();
    return 1;
}

int CH341SoftwareI2C::abortTransfer(void)
{
    if (failed())
    {
        if (_stretch_timeout_error)
            LOG("SoftwareI2C: clock stretch timeout (device 0x%02X), aborting transfer\n", _deviceID);
        busRecover();
    }
    else
    {
        stopBit();		// plain NACK: bus is in a defined state, just end it
    }
    return 0;
}

// Basic high level methods
int CH341SoftwareI2C::writeToRegister(uint8_t regAddress, uint8_t data, bool sendStopBit)
{
    return transfer(&regAddress, &data, 1, NULL, 0, sendStopBit);
}

int CH341SoftwareI2C::writeToRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // Least significant byte is written first, ie. buffer[0] sent first
    return transfer(&regAddress, buffer, count, NULL, 0, sendStopBit);
}

int CH341SoftwareI2C::writeToDevice(uint8_t data, bool sendStopBit)
{
    // Use with devices that do not use register addresses.
    return transfer(NULL, &data, 1, NULL, 0, sendStopBit);
}

int CH341SoftwareI2C::writeToDevice(uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // Use with devices that do not use register addresses.
    // Least significant byte is written first, ie. buffer[0] sent first
    return transfer(NULL, buffer, count, NULL, 0, sendStopBit);
}

int CH341SoftwareI2C::readFromRegister(uint8_t regAddress, uint8_t &data, bool sendStopBit)
{
    return transfer(&regAddress, NULL, 0, &data, 1, sendStopBit);
}

int CH341SoftwareI2C::readFromRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // Bytes are returned in <buffer>, which is assumed to be at least <count> bytes in size.
    return transfer(&regAddress, NULL, 0, buffer, count, sendStopBit);
}

int CH341SoftwareI2C::readFromDevice(uint8_t &data, bool sendStopBit)
{
    // Use this with devices that do not use register addresses.
    return transfer(NULL, NULL, 0, &data, 1, sendStopBit);
}

int CH341SoftwareI2C::readFromDevice(uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // Use this with devices that do not use register addresses.
    // Bytes are returned in <buffer>, which is assumed to be at least <count> bytes in size.
    return transfer(NULL, NULL, 0, buffer, count, sendStopBit);
}

// Other high level methods, kept for backwards compatibility with older
// versions of the library; new code should use the methods above
int CH341SoftwareI2C::write1bToRegister(uint8_t regAddress, uint8_t data, bool sendStopBit)
{
    return writeToRegister(regAddress, data, sendStopBit);
}

int CH341SoftwareI2C::writeBytesToRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    return writeToRegister(regAddress, buffer, count, sendStopBit);
}

int CH341SoftwareI2C::write1bToDevice(uint8_t data, bool sendStopBit)
{
    return writeToDevice(data, sendStopBit);
}

int CH341SoftwareI2C::writeBytesToDevice(uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    return writeToDevice(buffer, count, sendStopBit);
}

int CH341SoftwareI2C::read1bFromRegister(uint8_t regAddress, uint8_t* data, bool sendStopBit)
{
    return readFromRegister(regAddress, *data, sendStopBit);
}

int CH341SoftwareI2C::readBytesFromRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    return readFromRegister(regAddress, buffer, count, sendStopBit);
}

int CH341SoftwareI2C::read1bFromDevice(uint8_t* data, bool sendStopBit)
{
    return readFromDevice(*data, sendStopBit);
}

int CH341SoftwareI2C::readBytesFromDevice(uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    return readFromDevice(buffer, count, sendStopBit);
}

// Low level methods

int CH341SoftwareI2C::sclHi()
{
    unsigned long startTimer;

    // I2C pull-up resistor pulls SCL high in INPUT (Hi-Z) mode
    sclState = true;
    update();
    if (gpioError)
        return -1;

    // Check to make sure SCL pin has actually gone high before returning
    // Device may be pulling SCL low to delay transfer (clock stretching)

    // If SCL is not pulled high within a timeout period, then return anyway to avoid locking.
    startTimer = millis();
    while (millis() - startTimer < _stretch_timeout_delay)
    {
        int scl = GetInput(SCL_IN_PIN_ID);
        if (scl < 0)
            return -1;	// gpioError already set
        if (scl == 1)
            return 0;	// SCL high before timeout, return without error
    }
    // SCL did not go high within the timeout, so set error and return anyway.
    _stretch_timeout_error = 1;
    return -1;
}

void CH341SoftwareI2C::sclLo()
{
    sclState = false;
    update();
}

void CH341SoftwareI2C::sdaHi()
{
    sdaState = true;
    update();
}

void CH341SoftwareI2C::sdaLo()
{
    sdaState = false;
    update();
}

void CH341SoftwareI2C::startBit()    // Assume SDA already HIGH
{
    sdaHi();	// for a repeated START SDA must be released before SCL goes high
    sclHi();
    sdaLo();
    sclLo();
}

int CH341SoftwareI2C::writeAddress(uint8_t r_w)    // Assume SCL, SDA already LOW from startBit()
{
    // Sends address + R/W bit; the caller reads the ACK with checkAckBit()
    writeByte(static_cast<uint8_t>((_deviceID << 1) | (r_w ? 1 : 0)));
    return failed() ? -1 : 0;
}

uint8_t CH341SoftwareI2C::checkAckBit()   // Can also be used by controller to send NACK after last byte is read from device
{
    sdaHi();    // Release data line. This will cause a NACK from controller when reading bytes.
    if (sclHi() != 0)
    {
        sclLo();
        return 1;	// error: report as NACK, failed() tells the difference
    }
    int ack = GetInput(SDA_IN_PIN_ID);
    sclLo();
	return static_cast<uint8_t>((ack == 0) ? 0 : 1);	// read failure (-1) counts as NACK, never as ACK
}

void CH341SoftwareI2C::writeAck()    // Used by controller to ACK to device bewteen multi-byte reads
{
    sdaLo();
    sclHi();
    sclLo();
    sdaHi();  // Release the data line
}

void CH341SoftwareI2C::writeRegister(uint8_t reg_id)
{
    writeByte(reg_id);
}

void CH341SoftwareI2C::stopBit()    // Assume SCK is already LOW (from ack or data write)
{
    sdaLo();
    sclHi();
    sdaHi();
}

uint8_t CH341SoftwareI2C::read1Byte()
{
    uint8_t value = 0;
    for (int i=7; i>=0; i--)
    {
        if (sclHi() != 0)
        {
            sclLo();
            return 0;	// error flagged, failed() reports it
        }
        if (GetInput(SDA_IN_PIN_ID) == 1)
            value |= static_cast<uint8_t>(1<<i);
        sclLo();
        if (failed())
            return 0;
    }
    return value;
}

void CH341SoftwareI2C::writeByte(uint8_t data)
{
    for (int i=7; i>=0; i--)
    {
        if (data & (1<<i))
            sdaHi();
        else
            sdaLo();
        if (sclHi() != 0)
        {
            sclLo();
            break;	// error flagged, failed() reports it
        }
        sclLo();
    }
    sdaHi();  // Release the data line for ACK from device
}

unsigned long CH341SoftwareI2C::getStretchTimeout()
{
    return _stretch_timeout_delay;
}

void CH341SoftwareI2C::setStretchTimeout(unsigned long t)
{
    _stretch_timeout_delay = t;
}

int CH341SoftwareI2C::checkStretchTimeout()
{
    int retval;
    retval = _stretch_timeout_error;
    // Clear the value upon reading it.
    _stretch_timeout_error = 0;
    return retval;
}

uint8_t CH341SoftwareI2C::getDeviceID()
{
    return _deviceID;
}

void CH341SoftwareI2C::setDeviceID(uint8_t deviceid)
{
    // deviceid is the 7-bit I2C address
    _deviceID = deviceid;
}
