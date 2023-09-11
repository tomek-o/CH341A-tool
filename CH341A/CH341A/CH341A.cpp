//---------------------------------------------------------------------------


#pragma hdrstop

#include "CH341A.h"
#include "Log.h"
#include <windows.h>
#include "CH341PAR/LIB/C/CH341DLL.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

CH341A ch341a;

CH341A::~CH341A(void)
{
	Close();
}

int CH341A::Open(int index)
{
	if (this->index != INVALID_INDEX)
	{
		Close();
	}

	HANDLE handle = CH341OpenDevice(index);
	if (handle == INVALID_HANDLE_VALUE)
		return -1;

	this->index = index;

	return 0;
}

void CH341A::Close(void)
{
	if (index != INVALID_INDEX)
	{
		CH341CloseDevice(index);	
		index = INVALID_INDEX;
	}
}

int CH341A::I2CReadByte(uint8_t i2cAddr, uint8_t dataAddr, uint8_t &data)
{
	if (index == INVALID_INDEX)
	{
		return -1;
	}

	bool res = CH341ReadI2C(index, i2cAddr, dataAddr, &data);
	if (res == false)
	{
		return -2;
	}
	return 0;
}

int CH341A::ReadData(uint8_t *buffer, unsigned long &count)
{
	if (index == INVALID_INDEX)
	{
		count = 0;
		return -1;
	}
	bool res = CH341ReadData(index, buffer, &count);
	if (res == false)
	{
		count = 0;
		return -2;
	}
	return 0;
}

int CH341A::I2CRead(uint8_t *buffer, unsigned int count)
{
	if (index == INVALID_INDEX)
	{
		return -1;
	}
	bool res = CH341StreamI2C(index, 0, NULL, count, buffer);
	if (res == false)
	{
		return -2;
	}
	return 0;
}

int CH341A::I2CIssueStart(void)
{
	uint8_t mBuffer[ mCH341_PACKET_LENGTH ];
	unsigned long mLength;
	mBuffer[0] = mCH341A_CMD_I2C_STREAM;
	mBuffer[1] = mCH341A_CMD_I2C_STM_STA;
	mBuffer[2] = mCH341A_CMD_I2C_STM_END;
	mLength = 3;
	return CH341WriteData(index, mBuffer, &mLength) != true;
}

int CH341A::I2CIssueStop(void)
{
	uint8_t mBuffer[mCH341_PACKET_LENGTH];
	unsigned long mLength;
	mBuffer[0] = mCH341A_CMD_I2C_STREAM;
	mBuffer[1] = mCH341A_CMD_I2C_STM_STO;
	mBuffer[2] = mCH341A_CMD_I2C_STM_END;
	mLength = 3;
	return CH341WriteData(index, mBuffer, &mLength) != true;
}

int CH341A::I2COutByteCheckAck(uint8_t outByte)
{
	uint8_t mBuffer[mCH341_PACKET_LENGTH];
	unsigned long mLength, mInLen;
	mBuffer[0] = mCH341A_CMD_I2C_STREAM;
	mBuffer[1] = mCH341A_CMD_I2C_STM_OUT;
	mBuffer[2] = outByte;
	mBuffer[3] = mCH341A_CMD_I2C_STM_END;
	mLength = 4;
	mInLen = 0;
	if (CH341WriteRead( index, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, &mInLen, mBuffer )) {
		if (mInLen && (mBuffer[mInLen - 1] & 0x80) == 0)
			return 0;
	}
	return -1;
}

int CH341A::I2CCheckDev(uint8_t addr)
{
	uint8_t buf;
	if (addr >= 0x80)
	{
		LOG("I2CCheckDev: invalid I2C address (%u)\n", static_cast<unsigned int>(addr));
		return -2;
	}
#ifdef __BORLANDC__
#pragma warn -8071	// already checked for address overflow above
#endif
	buf = (addr << 1);
#ifdef __BORLANDC__
#pragma warn .8071
#endif

	I2CIssueStart();
	if(I2COutByteCheckAck(buf) == 0)
	{
		I2CIssueStop();
		return 0;
	}
	else
	{
		I2CIssueStop();
		return -1;
	}
}


