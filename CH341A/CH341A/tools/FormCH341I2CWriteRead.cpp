//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CWriteRead.h"
#include "CH341A.h"
#include "TabManager.h"
#include <vector>
#include <stdint.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CWriteRead *frmCH341I2CWriteRead;

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

//---------------------------------------------------------------------------
__fastcall TfrmCH341I2CWriteRead::TfrmCH341I2CWriteRead(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CWriteRead::btnReadClick(TObject *Sender)
{
	int count = StrToIntDef(cbReadBytesCount->Text, -1);
	if (count < 0)
	{
		MessageBox(this->Handle, "Invalid number of bytes to read", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	memoRead->Clear();

	std::vector<uint8_t> data;
	data.resize(count);
	int status = ch341a.I2CRead(&data[0], count);
	if (status != 0)
	{
		MessageBox(this->Handle, "Read error", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	AnsiString text;
	for (unsigned int i=0; i<data.size(); i++)
	{
		text.cat_printf("0x%02X", data[i]);
		if ((i % 7) == 7)
			text.cat_printf("\n");
		else
			text.cat_printf(" ");
	}
	memoRead->Text = text;	
}
//---------------------------------------------------------------------------
