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
		index(INVALID_INDEX)
	{
	}
	~CH341A(void);
	int Open(int index, const CH341AConf &cfg);
	void Close(void);
	bool IsOpened(void) const {
		return (index != INVALID_INDEX);
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

	int I2CWriteCommandReadByte(uint8_t i2cAddr, uint8_t command, uint8_t &data);
	int I2CWriteCommandReadBuffer(uint8_t i2cAddr, uint8_t command, uint8_t *data, unsigned int dataSize);

	int I2CWriteCommandWriteByte(uint8_t i2cAddr, uint8_t command, uint8_t data);

	int I2CWriteByte(uint8_t i2cAddr, uint8_t data);
	int I2CReadByte(uint8_t i2cAddr, uint8_t &data);

	int SetGpioOutputs(uint32_t direction, uint32_t dataOut);
	int GetGpioInputs(uint32_t &dataIn);
private:
	int index;
};

extern CH341A ch341a;

#endif
