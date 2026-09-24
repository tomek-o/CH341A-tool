//---------------------------------------------------------------------------

#ifndef CH341SoftwareI2CH
#define CH341SoftwareI2CH
//---------------------------------------------------------------------------

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

#include <stdint.h>

class CH341SoftwareI2C
{
public:
    CH341SoftwareI2C(void);
    /** Release the bus, check the RXD-SCL loopback and recover a stuck bus.
        \return 0 on success, -1 on CH341 GPIO failure, -2 if SCL input (RXD) reads
        low (loopback missing / SCL stuck), -3 if SDA stays low after recovery
    */
    int begin();
    /** Clock SCL until a slave stuck mid-byte releases SDA, then send STOP.
        \return 0 if both lines read high afterwards, -1 otherwise
    */
    int busRecover();

    // Return value convention:
    //  - high level methods (writeTo*, readFrom*, write1b*, read1b*, *Bytes*)
    //    return 1 on success and 0 on failure (NACK, clock stretch timeout or
    //    CH341 GPIO error) - as in the original SWI2C library
    //  - low level methods returning int (sclHi, writeAddress) return 0 on
    //    success and -1 on failure

    // Basic high level methods
    int writeToRegister(uint8_t regAddress, uint8_t data, bool sendStopBit = true);
    int writeToRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit = true);
    int writeToDevice(uint8_t data, bool sendStopBit = true);
    int writeToDevice(uint8_t* buffer, uint8_t count, bool sendStopBit = true);

    int readFromRegister(uint8_t regAddress, uint8_t &data, bool sendStopBit = true);
    int readFromRegister(uint8_t regAddress, uint8_t* buffer, uint8_t count, bool sendStopBit = true);
    int readFromDevice(uint8_t &data, bool sendStopBit = true);
    int readFromDevice(uint8_t* buffer, uint8_t count, bool sendStopBit = true);

    // Other high level methods for more specific use cases
    int write1bToRegister(uint8_t regAddress, uint8_t data, bool sendStopBit = true);
    int writeBytesToRegister(uint8_t regAddress, uint8_t* data, uint8_t count, bool sendStopBit = true);
    int write1bToDevice(uint8_t data, bool sendStopBit = true);
    int writeBytesToDevice(uint8_t* data, uint8_t count, bool sendStopBit = true);
    int read1bFromRegister(uint8_t regAddress, uint8_t* data, bool sendStopBit = true);
    int readBytesFromRegister(uint8_t regAddress, uint8_t* data, uint8_t count, bool sendStopBit = true);
    int read1bFromDevice(uint8_t* data, bool sendStopBit = true);
    int readBytesFromDevice(uint8_t* data, uint8_t count, bool sendStopBit = true);

    // Low level methods
    int sclHi();
    void sclLo();
    void sdaHi();
    void sdaLo();
    void startBit();
    int writeAddress(uint8_t r_w);
    uint8_t checkAckBit();
    void writeAck();
    void writeRegister(uint8_t regAddress);
    void stopBit();
    uint8_t read1Byte();
    void writeByte(uint8_t data);
    unsigned long getStretchTimeout();
    void setStretchTimeout(unsigned long t);
    int checkStretchTimeout();
    uint8_t getDeviceID();
    void setDeviceID(uint8_t deviceid);

private:
    bool failed(void) const;
    bool sendByteCheckAck(uint8_t data);
    int transfer(const uint8_t *regAddress, const uint8_t *wbuf, uint8_t wcount,
        uint8_t *rbuf, uint8_t rcount, bool sendStopBit);
    int abortTransfer(void);
    enum {DEFAULT_STRETCH_TIMEOUT = 500UL};   // ms timeout waiting for device to release SCL line
    uint8_t _deviceID;
    unsigned long _stretch_timeout_delay;
    unsigned int _stretch_timeout_error;
};

extern CH341SoftwareI2C ch341SoftwareI2C;


#endif

