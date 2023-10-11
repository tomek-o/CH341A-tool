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

unsigned int CH341A::GetMaxDataLengthInRequest(void)
{
	return mCH341A_CMD_I2C_STM_MAX;
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
	mBuffer[2] = outByte;	// I2C address, shifted
	mBuffer[3] = mCH341A_CMD_I2C_STM_END;
	mLength = 4;
	mInLen = 0;
	uint8_t mInBuf[16] = {0};
	/** \todo Weird/unexpected: despite mInLen = 0, ACK/output is written to mInBuf
	*/
	if (CH341WriteRead( index, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, &mInLen, mInBuf )) {
		if (mInLen && (mInBuf[mInLen - 1] & 0x80) == 0)
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

int CH341A::I2CWriteRead(uint8_t *writeBuffer, unsigned int writeCount, uint8_t *readBuffer, unsigned int readCount)
{
	if (index == INVALID_INDEX)
	{
		return -1;
	}

	// normalize for CH341 API
	if (writeBuffer == NULL)
		writeCount = 0;
	if (writeCount == 0)
		writeBuffer = NULL;

	if (readBuffer == NULL)
		readCount = 0;
	if (readCount == 0)
		readBuffer = NULL;

	if (CH341StreamI2C(index, writeCount, writeBuffer, readCount, readBuffer))
	{
		return 0;
	}
	readCount = 0;
	return -2;
}

int CH341A::I2CWriteCommandReadWord(uint8_t i2cAddr, uint8_t command, int16_t &data)
{
	uint8_t writeBuffer[2];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandReadWord: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
		return -2;
	}
#ifdef __BORLANDC__
#pragma warn -8071	// already checked for address overflow above
#endif
	writeBuffer[0] = (i2cAddr << 1);
#ifdef __BORLANDC__
#pragma warn .8071
#endif
	writeBuffer[1] = command;

	unsigned int readCount = sizeof(data);
	return I2CWriteRead(writeBuffer, sizeof(writeBuffer), (uint8_t*)&data, readCount);
}


#if 0
BOOL WINAPI PCF8574_WriteIO (// output the PCF8574 I / O
ULONG iIndex,       // ??device number designated CH341
ULONG iDeviceAddr,  // ??device address, the lowest order bit of the direction bit
ULONG iOutByte)     // prepare to write the I / O data
{                                                               // Could directly use CH341StreamI2C (iIndex, 2, mBuffer, 0, NULL) to achieve
    UCHAR mBuffer [mCH341_PACKET_LENGTH];
    ULONG mLength;

	mBuffer [0] = mCH341A_CMD_I2C_STREAM;                       // command code
	mBuffer [1] = mCH341A_CMD_I2C_STM_STA;                      // generate the start bit
	mBuffer [2] = (UCHAR) (mCH341A_CMD_I2C_STM_OUT | 2);        // output data, bit 5 - bit 0 is the length of 2 bytes
	mBuffer [3] = (UCHAR) (iDeviceAddr & 0xFE);                 // device address, write
	mBuffer [4] = (UCHAR) iOutByte;                             // I / O data
	mBuffer [5] = mCH341A_CMD_I2C_STM_STO;                      // stop bit generation
	mBuffer [6] = mCH341A_CMD_I2C_STM_END;                      // end of the current package in advance
	mLength = 7;
	return (CH341WriteData (iIndex, mBuffer, & mLength));       // write the data block
}

// input the PCF8574 I / O
BOOL WINAPI PCF8574_ReadIO (
ULONG iIndex,               // ??device number designated CH341
ULONG iDeviceAddr,          // ??device address, the lowest order bit of the direction bit
PUCHAR oInByte)             // points to a byte buffer is read back after the I / O data
{                                                               // Could directly use CH341StreamI2C (iIndex, 1, mBuffer, 1, oInByte) to achieve
	UCHAR mBuffer [mCH341_PACKET_LENGTH];
	ULONG mLength, mInLen;

	mBuffer [0] = mCH341A_CMD_I2C_STREAM;                       // command code
	mBuffer [1] = mCH341A_CMD_I2C_STM_STA;                      // generate the start bit
	mBuffer [2] = (UCHAR) (mCH341A_CMD_I2C_STM_OUT | 1);        // output data, bit 5 - bit 0 of length, 1 byte
	mBuffer [3] = (UCHAR) (iDeviceAddr | 0x01);                 // device address, read
	mBuffer [4] = (UCHAR) (mCH341A_CMD_I2C_STM_IN | 1);         // input data, bit 5 - bit 0 of length, 1 byte
	mBuffer [5] = mCH341A_CMD_I2C_STM_STO;                      // stop bit generation
	mBuffer [6] = mCH341A_CMD_I2C_STM_END;                      // end of the current package in advance
	mLength = 7;
	mInLen = 0;
																// execute the command stream, and then input the first output
	if (CH341WriteRead (iIndex, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, & mInLen, mBuffer)) {
		if (mInLen) {
		*OInByte = mBuffer[mInLen - 1]; // return the data
		return (TRUE);
		}
	}
	return (FALSE);
}
#endif


