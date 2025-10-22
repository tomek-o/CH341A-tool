//---------------------------------------------------------------------------
#pragma hdrstop

/*
	Requires RXD and SCL lines connected together!
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

bool sclState = false;
bool sdaState = false;

void update(void)
{
    uint32_t direction = (1u << SCL_OUT_PIN_ID) | (1u << SDA_OUT_PIN_ID);
    uint32_t value = 0;
    if (sclState)
        value |= (1u << SCL_OUT_PIN_ID);
    if (sdaState)
        value |= (1u << SDA_OUT_PIN_ID);

    ch341a.SetGpioOutputs(direction, value);
    //Sleep(10);
}

bool GetSclInput(void)
{
    uint32_t dataIn = 0;

    int status = ch341a.GetGpioInputs(dataIn);
    if (status != 0)
    {
        LOG("Reading inputs failed!\n");
        return false;
    }
    if (dataIn & (1u << SCL_IN_PIN_ID))
        return true;
    return false;
}

bool GetSdaInput(void)
{
    uint32_t dataIn = 0;

    int status = ch341a.GetGpioInputs(dataIn);
    if (status != 0)
    {
        LOG("Reading inputs failed!\n");
        return false;
    }
    if (dataIn & (1u << SDA_IN_PIN_ID))
        return true;
    return false;
}

#define millis timeGetTime

}	// namespace

CH341SoftwareI2C ch341SoftwareI2C;

CH341SoftwareI2C::CH341SoftwareI2C(void)
{
    _stretch_timeout_delay = DEFAULT_STRETCH_TIMEOUT;
    _stretch_timeout_error = 0;
}

void CH341SoftwareI2C::begin()
{
    sclState = true;
    sdaState = true;
    update();
}

// Basic high level methods
int CH341SoftwareI2C::writeToRegister(uint8_t regAddress, uint8_t data, bool sendStopBit)
{
    startBit();
    writeAddress(0);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    writeRegister(regAddress);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    writeByte(data);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}

int CH341SoftwareI2C::writeToRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // Writes <count> bytes after sending device address and register address.
    // Least significant byte is written first, ie. buffer[0] sent first

    startBit();
    writeAddress(0);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    writeRegister(regAddress);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    // Loop through bytes in the buffer
    for (uint8_t i = 0; i < count; i++)
    {
        writeByte(buffer[i]);
        if (checkAckBit())
        {
            stopBit();    // Immediately end transmission and return 0 if NACK detected
            return 0;
        }
    }
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}
int CH341SoftwareI2C::writeToDevice(uint8_t data, bool sendStopBit)
{
    // Use with devices that do not use register addresses.

    startBit();
    writeAddress(0);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    writeByte(data);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}

int CH341SoftwareI2C::writeToDevice(uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // Use with devices that do not use register addresses.
    // Writes <count> bytes after sending device address.
    // Least significant byte is written first, ie. buffer[0] sent first

    startBit();
    writeAddress(0);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    // Loop through bytes in the buffer
    for (uint8_t i = 0; i < count; i++)
    {
        writeByte(buffer[i]); // LSB
        if (checkAckBit())
        {
            stopBit();    // Immediately end transmission and return 0 if NACK detected
            return 0;
        }
    }
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}

int CH341SoftwareI2C::readFromRegister(uint8_t regAddress, uint8_t &data, bool sendStopBit)
{
    // This method uses pass-by-reference for the data byte
    startBit();
    writeAddress(0); // 0 == Write bit
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    writeRegister(regAddress);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    startBit();
    writeAddress(1); // 1 == Read bit
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    data = read1Byte();
    checkAckBit(); // Controller needs to send NACK when done reading data
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}

int CH341SoftwareI2C::readFromRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // Reads <count> bytes after sending device address and register address.
    // Bytes are returned in <buffer>, which is assumed to be at least <count> bytes in size.

    startBit();
    writeAddress(0); // 0 == Write bit
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    writeRegister(regAddress);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    startBit();
    writeAddress(1); // 1 == Read bit
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    // Loop through bytes in the buffer
    for (uint8_t i = 0; i < count; i++)
    {
        buffer[i] = read1Byte();
        if (i < (count-1))
        {
            writeAck();
        }
        else   // Last byte needs a NACK
        {
            checkAckBit(); // Controller needs to send NACK when done reading data
        }
    }
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}

int CH341SoftwareI2C::readFromDevice(uint8_t &data, bool sendStopBit)
{
    // Use this with devices that do not use register addresses.

    startBit();
    writeAddress(1); // 1 == Read bit
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    data = read1Byte();
    checkAckBit(); // Controller needs to send NACK when done reading data
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}

int CH341SoftwareI2C::readFromDevice(uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // Use this with devices that do not use register addresses.
    // Reads <count> bytes after sending device address.
    // Bytes are returned in <buffer>, which is assumed to be at least <count> bytes in size.

    startBit();
    writeAddress(1); // 1 == Read bit
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    // Loop through bytes in the buffer
    for (uint8_t i = 0; i < count; i++)
    {
        buffer[i] = read1Byte();
        if (i < (count-1))
        {
            writeAck();
        }
        else   // Last byte needs a NACK
        {
            checkAckBit(); // Controller needs to send NACK when done reading data
        }
    }
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}

// Other high level methods for more specific use cases
// write1bToRegister is here for backwards compatibility with older versions of the library
// New code should use writeToRegister()
int CH341SoftwareI2C::write1bToRegister(uint8_t regAddress, uint8_t data, bool sendStopBit)
{
    return writeToRegister(regAddress, data, sendStopBit);
}

int CH341SoftwareI2C::writeBytesToRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // writeBytesToRegister is here for backwards compatibility with older versions of the library
    // New code should use writeToRegister()
    // Least significant byte is written first, ie. buffer[0] sent first
    return writeToRegister(regAddress, buffer, count, sendStopBit);
}

int CH341SoftwareI2C::write1bToDevice(uint8_t data, bool sendStopBit)
{
    // write1bToDevice is here for backwards compatibility with older versions of the library
    // New code should use writeToDevice()
    // Use with devices that do not use register addresses.
    return writeToDevice(data, sendStopBit);
}

int CH341SoftwareI2C::writeBytesToDevice(uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // writeBytesToDevice is here for backwards compatibility with older versions of the library
    // New code should use writeToDevice()
    // Use with devices that do not use register addresses.
    // Writes <count> bytes after sending device address.
    // Least significant byte is written first, ie. buffer[0] sent first
    return writeToDevice(buffer, count, sendStopBit);
}

int CH341SoftwareI2C::read1bFromRegister(uint8_t regAddress, uint8_t* data, bool sendStopBit)
{
    startBit();
    writeAddress(0); // 0 == Write bit
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    writeRegister(regAddress);
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    startBit();
    writeAddress(1); // 1 == Read bit
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    *data = read1Byte();
    checkAckBit(); // Controller needs to send NACK when done reading data
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}

int CH341SoftwareI2C::readBytesFromRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // readBytesFromRegister is here for backwards compatibility with older versions of the library
    // New code should use readFromRegister()
    // Bytes are returned in <buffer>, which is assumed to be at least <count> bytes in size.
    return readFromRegister(regAddress, buffer, count, sendStopBit);
}

int CH341SoftwareI2C::read1bFromDevice(uint8_t* data, bool sendStopBit)
{
    // Use this with devices that do not use register addresses.

    startBit();
    writeAddress(1); // 1 == Read bit
    if (checkAckBit())
    {
        stopBit();    // Immediately end transmission and return 0 if NACK detected
        return 0;
    }
    *data = read1Byte();
    checkAckBit(); // Controller needs to send NACK when done reading data
    if (sendStopBit) stopBit();
    return 1;  // Return 1 if no NACKs
}

int CH341SoftwareI2C::readBytesFromDevice(uint8_t* buffer, uint8_t count, bool sendStopBit)
{
    // readBytesFromDevice is here for backwards compatibility with older versions of the library
    // New code should use readFromDevice()
    // Use this with devices that do not use register addresses.
    // Reads <count> bytes after sending device address.
    // Bytes are returned in <buffer>, which is assumed to be at least <count> bytes in size.
    return readFromDevice(buffer, count, sendStopBit);
}

// Low level methods

int CH341SoftwareI2C::sclHi()
{
    unsigned long startTimer;

    // I2C pull-up resistor pulls SCL high in INPUT (Hi-Z) mode
    sclState = true;
    update();

    // Check to make sure SCL pin has actually gone high before returning
    // Device may be pulling SCL low to delay transfer (clock stretching)

    // If SCL is not pulled high within a timeout period, then return anyway to avoid locking.
    startTimer = millis();
    while (millis() - startTimer < _stretch_timeout_delay)
    {
        if (GetSclInput())
            return 0; // SCL high before timeout, return without error
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
    sclHi();
    sdaLo();
    sclLo();
}

int CH341SoftwareI2C::writeAddress(uint8_t r_w)    // Assume SCL, SDA already LOW from startBit()
{
    int status;
    // address bits: 6...0
	for (int i=6; i>=0; i--)
    {
        if (_deviceID & (1<<i))
            sdaHi();
        else
            sdaLo();
        status = sclHi();
        if (status)
            return status;
        sclLo();
	}

    if (r_w == 1) sdaHi();         // R/W bit
    else sdaLo();
    sclHi();
    sclLo();
    sdaHi();    // Release the data line for ACK signal from device
    return 0;
}

uint8_t CH341SoftwareI2C::checkAckBit()   // Can also be used by controller to send NACK after last byte is read from device
{
    uint8_t ack;
    sdaHi();    // Release data line. This will cause a NACK from controller when reading bytes.
    sclHi();
    ack = GetSdaInput();
    sclLo();
    return ack;
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
		sclHi();
		if (GetSdaInput())
			value += static_cast<uint8_t>(1<<i);
		sclLo();
	}
    return value;
}

void CH341SoftwareI2C::writeByte(uint8_t data)
{
    if (data & 0x80) sdaHi();     // bit 7
    else sdaLo();
    sclHi();
    sclLo();
    if (data & 0x40) sdaHi();     // bit 6
    else sdaLo();
    sclHi();
    sclLo();
    if (data & 0x20) sdaHi();     // bit 5
    else sdaLo();
    sclHi();
    sclLo();
    if (data & 0x10) sdaHi();     // bit 4
    else sdaLo();
    sclHi();
    sclLo();
    if (data & 0x08) sdaHi();     // bit 3
    else sdaLo();
    sclHi();
    sclLo();
    if (data & 0x04) sdaHi();     // bit 2
    else sdaLo();
    sclHi();
    sclLo();
    if (data & 0x02) sdaHi();     // bit 1
    else sdaLo();
    sclHi();
    sclLo();
    if (data & 0x01) sdaHi();     // bit 0
    else sdaLo();
    sclHi();
    sclLo();
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
