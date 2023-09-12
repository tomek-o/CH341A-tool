//---------------------------------------------------------------------------

#ifndef CH341AH
#define CH341AH
//---------------------------------------------------------------------------

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
	int Open(int index);
	void Close(void);
	bool IsOpened(void) const {
		return (index != INVALID_INDEX);
	}

	int I2CReadByte(uint8_t i2cAddr, uint8_t dataAddr, uint8_t &data);

	int ReadData(uint8_t *buffer, unsigned long &count);

	int I2CRead(uint8_t *buffer, unsigned int count);

	int I2CIssueStart(void);
	int I2CIssueStop(void);
	int I2COutByteCheckAck(uint8_t outByte);
	int I2CCheckDev(uint8_t addr);

private:
	int index;
};

extern CH341A ch341a;

#endif