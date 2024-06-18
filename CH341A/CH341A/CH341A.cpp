//---------------------------------------------------------------------------


#pragma hdrstop

#include "CH341A.h"
#include "Log.h"
#include <windows.h>
#include "CH341PAR/LIB/C/CH341DLL.h"
#include <System.hpp>
#include <assert.h>
#include <vector>

//---------------------------------------------------------------------------

#pragma package(smart_init)

CH341A ch341a;

CH341A::~CH341A(void)
{
	Close();
}

int CH341A::Open(int index, const CH341AConf &cfg)
{
	if (this->index != INVALID_INDEX)
	{
		Close();
	}

	HANDLE handle = CH341OpenDevice(index);
	if (handle == INVALID_HANDLE_VALUE)
	{
		LOG("Failed to open CH341A\n");
		return -1;
	}

	this->index = index;

	ULONG ver = CH341GetVerIC(index);
	AnsiString chipVer;
	if (ver == IC_VER_CH341A) {
		chipVer.sprintf("0x%08X (CH341A)", ver);
	} else if (ver == IC_VER_CH341A3) {
		chipVer.sprintf("0x%08X (CH341A3)", ver);
	} else {
		chipVer.sprintf("0x%08X (unknown)", ver);
	}
	LOG("CH341A opened, chip version %s\n", chipVer.c_str());

	bool res;
	ULONG iMode = 0;

    LOG("Setting I2C speed to [%s]\n", CH341AConf::getI2CSpeedDescription(cfg.i2cSpeed));
	switch (cfg.i2cSpeed)
	{
	case CH341AConf::I2C_SPEED_20K:
		break;
	case CH341AConf::I2C_SPEED_100K:
		iMode |= 0x01;
		break;
	case CH341AConf::I2C_SPEED_400K:
		iMode |= 0x02;
		break;
	case CH341AConf::I2C_SPEED_750K:
		iMode |= 0x03;
		break;
	default:
		assert(!"Unhandled I2C speed mode!");
	}

	{
		iMode |= 0x80;
		LOG("Setting SPI mode / bit order to MSB first\n");
	}

	res = CH341SetStream(index, iMode);
	if (res == false)
	{
		LOG("CH341A: failed to set stream configuration!\n");
		CH341CloseDevice(index);	
		this->index = INVALID_INDEX;		
		return -2;
	}

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

// Output one byte of data and check whether the response is valid
int CH341A::I2COutByteCheckAck(uint8_t outByte)
{
	uint8_t mBuffer[mCH341_PACKET_LENGTH];
	unsigned long mLength, mInLen;
	mBuffer[0] = mCH341A_CMD_I2C_STREAM;
	mBuffer[1] = mCH341A_CMD_I2C_STM_OUT;	// Output data, bit 5-bit 0 is the length, if the length is 0, only one byte is sent and a response is returned
	mBuffer[2] = outByte;					// I2C address, shifted
	mBuffer[3] = mCH341A_CMD_I2C_STM_END;	// The current packet ends early
	mLength = 4;
	mInLen = 0;
	uint8_t mInBuf[16] = {0};
	/** \todo 	Weird/unexpected: despite setting mInLen = 0, CH341 API sets mInLen = 1
				and ACK/output is written to mInBuf
	*/
	if (CH341WriteRead( index, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, &mInLen, mInBuf )) {
		// Bit 7 of the returned data represents the ACK response bit, ACK=0 is valid	
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

	/** \todo CH341StreamI2C is not failing if target device is not connected to I2C,
	returning series of 0xFF bytes. Same goes for CH341PAR.exe from CH341 EVT package.
	*/
	if (CH341StreamI2C(index, writeCount, writeBuffer, readCount, readBuffer))
	{
		return 0;
	}
	return -2;
}

int CH341A::I2CWriteRead2(uint8_t addr, uint8_t *writeBuffer, unsigned int writeCount, uint8_t *readBuffer, unsigned int readCount)
{
	if (index == INVALID_INDEX)
	{
		return -1;
	}

	uint8_t addrBuf;
	if (addr >= 0x80)
	{
		LOG("I2CWriteRead2: invalid I2C address (%u)\n", static_cast<unsigned int>(addr));
		return -2;
	}
#ifdef __BORLANDC__
#pragma warn -8071	// already checked for address overflow above
#endif
	addrBuf = (addr << 1);
#ifdef __BORLANDC__
#pragma warn .8071
#endif

	uint8_t mBuffer[mCH341_PACKET_LENGTH];
	unsigned long mLength, mInLen;
	unsigned int mBufferPos;
	uint8_t mInBuf[16] = {0};

	mBufferPos = 0;
	mBuffer[mBufferPos++] = mCH341A_CMD_I2C_STREAM;
	mBuffer[mBufferPos++] = mCH341A_CMD_I2C_STM_OUT;	// Output data, bit 5-bit 0 is the length, if the length is 0, only one byte is sent and a response is returned
	mBuffer[mBufferPos++] = addrBuf;					// I2C address, shifted
	mLength = mBufferPos++;
	(void)mBufferPos;
	mInLen = 0;
	if (CH341WriteRead( index, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, &mInLen, mInBuf )) {
		// Bit 7 of the returned data represents the ACK response bit, ACK=0 is valid
		if (mInLen && (mInBuf[mInLen - 1] & 0x80) != 0) {
			LOG("I2CWriteRead2: no ACK after adress is sent!\n");
			return -3;
		}
	}

	return I2CWriteRead(writeBuffer, writeCount, readBuffer, readCount);
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

int CH341A::I2CWriteCommandReadU16FromMsb(uint8_t i2cAddr, uint8_t command, uint16_t &data)
{
	uint8_t writeBuffer[2];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandReadU16FromMsb: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
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
	int status = I2CWriteRead(writeBuffer, sizeof(writeBuffer), (uint8_t*)&data, readCount);
#ifdef __BORLANDC__
#pragma warn -8071
#endif
	data = (data << 8) | (data >> 8);
#ifdef __BORLANDC__
#pragma warn .8071
#endif
	return status;
}

int CH341A::I2CWriteCommandReadU32FromMsb(uint8_t i2cAddr, uint8_t command, uint32_t &data)
{
	uint8_t writeBuffer[2];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandReadU16FromMsb: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
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
	int status = I2CWriteRead(writeBuffer, sizeof(writeBuffer), (uint8_t*)&data, readCount);
#ifdef __BORLANDC__
#pragma warn -8071
#endif
	data = 	((data>>24)&0xff) | 	// move byte 3 to byte 0
			((data<<8)&0xff0000) |	// move byte 1 to byte 2
			((data>>8)&0xff00) |	// move byte 2 to byte 1
			((data<<24)&0xff000000);// byte 0 to byte 3
#ifdef __BORLANDC__
#pragma warn .8071
#endif
	return status;
}

int CH341A::I2CWriteCommandReadWord2(uint8_t i2cAddr, uint8_t command, int16_t &data)
{
	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandReadWord: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
		return -2;
	}

	unsigned int readCount = sizeof(data);
	return I2CWriteRead2(i2cAddr, &command, sizeof(command), (uint8_t*)&data, readCount);
}

int CH341A::I2CWriteCommandReadByte(uint8_t i2cAddr, uint8_t command, uint8_t &data)
{
	uint8_t writeBuffer[2];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandReadByte: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
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
	return I2CWriteRead(writeBuffer, sizeof(writeBuffer), &data, readCount);
}

int CH341A::I2CWriteCommandReadBuffer(uint8_t i2cAddr, uint8_t command, uint8_t *data, unsigned int dataSize)
{
	uint8_t writeBuffer[2];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandReadBuffer: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
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

	return I2CWriteRead(writeBuffer, sizeof(writeBuffer), data, dataSize);
}

int CH341A::I2CWriteCommandWriteByte(uint8_t i2cAddr, uint8_t command, uint8_t data)
{
	uint8_t writeBuffer[3];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandWriteByte: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
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
	writeBuffer[2] = data;

	unsigned int readCount = 0;
	return I2CWriteRead(writeBuffer, sizeof(writeBuffer), NULL, readCount);
}

int CH341A::I2CWriteCommandWrite2B(uint8_t i2cAddr, uint8_t command, uint8_t data1, uint8_t data2)
{
	uint8_t writeBuffer[4];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandWrite2B: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
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
	writeBuffer[2] = data1;
	writeBuffer[3] = data2;

	unsigned int readCount = 0;
	return I2CWriteRead(writeBuffer, sizeof(writeBuffer), NULL, readCount);
}

int CH341A::I2CWriteCommandWriteBytes(uint8_t i2cAddr, uint8_t command, const uint8_t *data, unsigned int count)
{
	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandWriteBytes: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
		return -2;
	}

	std::vector<uint8_t> writeBuffer;
	writeBuffer.resize(2 + count);
#ifdef __BORLANDC__
#pragma warn -8071	// already checked for address overflow above
#endif
	writeBuffer[0] = (i2cAddr << 1);
#ifdef __BORLANDC__
#pragma warn .8071
#endif
	writeBuffer[1] = command;
	memcpy(&writeBuffer[2], data, count);

	unsigned int readCount = 0;
	return I2CWriteRead(&writeBuffer[0], writeBuffer.size(), NULL, readCount);
}


int CH341A::I2CWriteCommandWriteUintFromMsb(uint8_t i2cAddr, uint8_t command, uint32_t value)
{
	uint8_t writeBuffer[6];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandWriteUintFromMsb: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
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
	writeBuffer[2] = (uint8_t)(value >> 24); // value highest byte
	writeBuffer[3] = (uint8_t)(value >> 16);
	writeBuffer[4] = (uint8_t)(value >>  8);
	writeBuffer[5] = (uint8_t)(value);       // value lowest byte

	unsigned int readCount = 0;
	return I2CWriteRead(writeBuffer, sizeof(writeBuffer), NULL, readCount);
}

int CH341A::I2CWriteCommandReadBytes(uint8_t i2cAddr, uint8_t command, uint8_t *data, unsigned int count)
{
	uint8_t writeBuffer[1];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteCommandReadBytes: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
		return -2;
	}
#ifdef __BORLANDC__
#pragma warn -8071	// already checked for address overflow above
#endif
	writeBuffer[0] = (i2cAddr << 1);
#ifdef __BORLANDC__
#pragma warn .8071
#endif

	return I2CWriteRead(writeBuffer, sizeof(writeBuffer), data, count);
}




int CH341A::I2CWriteByte(uint8_t i2cAddr, uint8_t data)
{
	uint8_t writeBuffer[2];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CWriteByte: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
		return -2;
	}
#ifdef __BORLANDC__
#pragma warn -8071	// already checked for address overflow above
#endif
	writeBuffer[0] = (i2cAddr << 1);
#ifdef __BORLANDC__
#pragma warn .8071
#endif
	writeBuffer[1] = data;

	unsigned int readCount = 0;
	return I2CWriteRead(writeBuffer, sizeof(writeBuffer), NULL, readCount);
}

int CH341A::I2CReadByte(uint8_t i2cAddr, uint8_t &data)
{
	uint8_t writeBuffer[1];

	if (i2cAddr >= 0x80)
	{
		LOG("I2CReadByte: invalid I2C address (%u)\n", static_cast<unsigned int>(i2cAddr));
		return -2;
	}
#ifdef __BORLANDC__
#pragma warn -8071	// already checked for address overflow above
#endif
	writeBuffer[0] = (i2cAddr << 1);
#ifdef __BORLANDC__
#pragma warn .8071
#endif

	unsigned int readCount = sizeof(data);
	return I2CWriteRead(writeBuffer, sizeof(writeBuffer), &data, readCount);
}

int CH341A::SetGpioOutputs(uint32_t direction, uint32_t dataOut)
{
	if (index == INVALID_INDEX)
	{
		return -1;
	}
	if (CH341SetOutput(index, 0xFFFFFFFF, direction, dataOut))
		return 0;
	return -2;
}

int CH341A::GetGpioInputs(uint32_t &dataIn)
{
	dataIn = 0;
	if (index == INVALID_INDEX)
	{
		return -1;
	}
	unsigned long val = 0;
	if (CH341GetInput(index, &val))
	{
		dataIn = val;
		return 0;
	}
	return -2;
}

int CH341A::SpiTransfer(uint8_t *buffer, unsigned int count)
{
	if (index == INVALID_INDEX)
	{
		return -1;
	}
	if (CH341StreamSPI4(index, 0x80 /* CS control using CS0 */, count, buffer))
	{
		return 0;
	}
	return -2;
}

int CH341A::SpiTransfer3Wire(uint8_t *buffer, unsigned int count)
{
	if (index == INVALID_INDEX)
	{
		return -1;
	}
	if (CH341StreamSPI3(index, 0x80 /* CS control using CS0 */, count, buffer))
	{
		return 0;
	}
	return -2;
}

int CH341A::SpiTransferBitStream(uint8_t *buffer, unsigned int count)
{
	if (index == INVALID_INDEX)
	{
		return -1;
	}
	if (CH341BitStreamSPI(index, count, buffer))
	{
		return 0;
	}
	return -2;
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


BOOL WINAPI IIC_OutBlockSkipAck(ULONG iIndex, //Specify CH341 device serial number
	ULONG iOutLength, //The number of data bytes to be written, must be less than 29 bytes at a time
	PVOID iOutBuffer ) // Points to a buffer to place the data to be written out
{
	UCHAR mBuffer[mCH341_PACKET_LENGTH];
	ULONGmLength;
	if ( iOutLength == 0 || iOutLength > ( mCH341_PACKET_LENGTH - 1 - 1 - 1 ) )
		return( FALSE );
	mBuffer[ 0 ] = mCH341A_CMD_I2C_STREAM; // Command code
	mBuffer[ 1 ] = (UCHAR)( mCH341A_CMD_I2C_STM_OUT | iOutLength ); // Output data, bit 5-bit 0 is the length
	memcpy( &mBuffer[2], iOutBuffer, iOutLength ); // data
	mBuffer[ 1 + 1 + iOutLength ] = mCH341A_CMD_I2C_STM_END; // The current packet ends early
	mLength = 1 + 1 + iOutLength + 1;
	return( CH341WriteData( iIndex, mBuffer, &mLength ) ); // Write out the data block
}

BOOL WINAPI IIC_InBlockByAck( //Input data block, each input byte will generate a valid response
	ULONG iIndex, //Specify CH341 device serial number
	ULONG iInLength, //The number of data bytes to be read must be less than 32 bytes at a time
	PVOID oInBuffer ) // Points to a buffer, and returns the read data
{
	UCHAR mBuffer[mCH341_PACKET_LENGTH];
	ULONG mLength, mInLen;
	if ( iInLength == 0 || iInLength > mCH341A_CMD_I2C_STM_MAX ) return( FALSE );
	mBuffer[ 0 ] = mCH341A_CMD_I2C_STREAM; // Command code
	mBuffer[ 1 ] = (UCHAR)( mCH341A_CMD_I2C_STM_IN | iInLength ); //Input data, bit 5-bit 0 is the length
	mBuffer[ 2 ] = mCH341A_CMD_I2C_STM_END; // The current packet ends early
	mLength = 3;
	mInLen = 0;
	if ( CH341WriteRead( iIndex, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, &mInLen, mBuffer ) ) { // Execute the data flow command, output first and then input
		if ( mInLen == iInLength ) {
			memcpy( oInBuffer, &mBuffer[0], iInLength ); // data
			return(TRUE);
		}
	}
	return(FALSE);
}

BOOL WINAPI IIC_InByteNoAck( // Input one byte of data, but no response is generated
	ULONG iIndex, //Specify CH341 device serial number
	PUCHAR oInByte ) // Points to a byte buffer, and returns the read data
{
	UCHAR mBuffer[mCH341_PACKET_LENGTH];
	ULONG mLength, mInLen;
	mBuffer[ 0 ] = mCH341A_CMD_I2C_STREAM; // Command code
	mBuffer[ 1 ] = mCH341A_CMD_I2C_STM_IN; // Input data, bit 5-bit 0 is the length, if the length is 0, only one byte will be received and no response will be sent.
	mBuffer[ 2 ] = mCH341A_CMD_I2C_STM_END; // The current packet ends early
	mLength = 3;
	mInLen = 0;
	if ( CH341WriteRead( iIndex, mLength, mBuffer, mCH341A_CMD_I2C_STM_MAX, 1, &mInLen, mBuffer ) ) { // Execute the data flow command, output first and then input
		if ( mInLen ) {
			*oInByte = mBuffer[ mInLen - 1 ]; // data
			return(TRUE);
		}
	}
	return(FALSE);
}


#endif


