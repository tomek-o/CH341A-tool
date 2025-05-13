//---------------------------------------------------------------------------

#ifndef CH341AH
#define CH341AH
//---------------------------------------------------------------------------

#include "CH341AConf.h"
#include <stdint.h>

class CH341A
{
public:
	enum { INVALID_INDEX = -1 };

	CH341A(void):
		index(INVALID_INDEX),
		i2cSpeed(CH341AConf::I2C_SPEED_20K)		
	{
	}
	~CH341A(void);
	int Open(int index, const CH341AConf &cfg);
	void Close(void);
	bool IsOpened(void) const {
		return (index != INVALID_INDEX);
	}

	enum CH341AConf::I2CSpeed GetI2CSpeed(void) const {
    	return i2cSpeed;
	}

	static unsigned int GetMaxDataLengthInRequest(void);

	int I2CReadByte(uint8_t i2cAddr, uint8_t dataAddr, uint8_t &data);

	int ReadData(uint8_t *buffer, unsigned long &count);

	int I2CRead(uint8_t *buffer, unsigned int count);

	int I2CIssueStart(void);
	int I2CIssueStop(void);
	int I2COutByteCheckAck(uint8_t outByte);
	int I2CCheckDev(uint8_t addr);

	int I2CWriteRead(uint8_t *writeBuffer, unsigned int writeCount, uint8_t *readBuffer, unsigned int readCount);

	/** Separate address (7-bit) from write buffer, check for ACK after address */
	int I2CWriteRead2(uint8_t addr, uint8_t *writeBuffer, unsigned int writeCount, uint8_t *readBuffer, unsigned int readCount);

	int I2CWriteCommandReadWord(uint8_t i2cAddr, uint8_t command, int16_t &data);
	int I2CWriteCommandReadWord2(uint8_t i2cAddr, uint8_t command, int16_t &data);
	int I2CWriteCommandReadU16FromMsb(uint8_t i2cAddr, uint8_t command, uint16_t &data);
	int I2CWriteCommandReadU32FromMsb(uint8_t i2cAddr, uint8_t command, uint32_t &data);

	int I2CWriteCommandReadByte(uint8_t i2cAddr, uint8_t command, uint8_t &data);
	int I2CWriteCommandReadBuffer(uint8_t i2cAddr, uint8_t command, uint8_t *data, unsigned int dataSize);

	int I2CWriteCommandWriteByte(uint8_t i2cAddr, uint8_t command, uint8_t data);
	int I2CWriteCommandWrite2B(uint8_t i2cAddr, uint8_t command, uint8_t data1, uint8_t data2);
	int I2CWriteCommandWriteBytes(uint8_t i2cAddr, uint8_t command, const uint8_t *data, unsigned int count);
	int I2CWriteCommandWriteUintFromMsb(uint8_t i2cAddr, uint8_t command, uint32_t value);
	int I2CWriteCommandReadBytes(uint8_t i2cAddr, uint8_t command, uint8_t *data, unsigned int count);

	int I2CWriteByte(uint8_t i2cAddr, uint8_t data);
	int I2CReadByte(uint8_t i2cAddr, uint8_t &data);

	int I2CWriteBytes(uint8_t i2cAddr, const uint8_t *data, unsigned int count);
	int I2CReadBytes(uint8_t i2cAddr, uint8_t *data, unsigned int count);

	int SetGpioOutputs(uint32_t direction, uint32_t dataOut);
	int GetGpioInputs(uint32_t &dataIn);

	/** Regular SPI with 4 lines */
	int SpiTransfer(void *buffer, unsigned int count);

	/** SPI transfer with 3-line interface (bidirectional data line)
		- like used by DS1626 and maybe DS1302.
		This uses SCL + SDA + D0 (CS) pins.
	*/
	int SpiTransfer3Wire(uint8_t *buffer, unsigned int count);

	/** SPI-like transfer with 5 output lines and 2 input lines
	*/
	int SpiTransferBitStream(uint8_t *buffer, unsigned int count);

private:
	int index;
	enum CH341AConf::I2CSpeed i2cSpeed;
};

extern CH341A ch341a;

#endif
