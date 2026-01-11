/** \brief Reading temperature from LM75 I2C sensor
*/

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CLm75.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CLm75 *frmCH341I2CLm75;
//---------------------------------------------------------------------------

namespace
{

enum { LM75B_SLAVE_FIRST_ADDR = 0x48 };
enum { LM75B_SLAVE_LAST_ADDR = 0x4F };

enum { LM75B_REG_CONF = 0x01 };		/**< configure register */
enum { LM75B_REG_TEMP = 0x00 };		/**< temperature register */
enum { LM75B_REG_TOS = 0x03 };		/**< TOS register */
enum { LM75B_REG_THYST = 0x02 };	/**< THYST register */

float BufToTemperature(const unsigned char *buffer)
{
	int16_t val;
	unsigned char* ptr = reinterpret_cast<unsigned char*>(&val);
	ptr[0] = buffer[1];
	ptr[1] = buffer[0];
	
	return static_cast<float>(val) / 32 / 8;
}

}	// namespace


__fastcall TfrmCH341I2CLm75::TfrmCH341I2CLm75(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, 1u << ToolGroupSensors);

	for (unsigned int i=LM75B_SLAVE_FIRST_ADDR; i<=LM75B_SLAVE_LAST_ADDR; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbI2CAddress->Items->Add(text);
	}
	cbI2CAddress->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CLm75::btnReadClick(TObject *Sender)
{
	Read();
}

void TfrmCH341I2CLm75::Read(void)
{
	BtnController btnCtrl(btnRead);

	edTemperature->Text = "";

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = static_cast<uint8_t>(LM75B_SLAVE_FIRST_ADDR + cbI2CAddress->ItemIndex);

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending device address!";
		return;
	}

	uint8_t writeBuffer[2];
#ifdef __BORLANDC__
#pragma warn -8071	// address is limited, remove warning about possible overflow
#endif
	writeBuffer[0] = address << 1;
#ifdef __BORLANDC__
#pragma warn .8071
#endif
	writeBuffer[1] = LM75B_REG_TEMP;
	uint8_t readBuffer[2] = {0};

	status = ch341a.I2CWriteRead(writeBuffer, sizeof(writeBuffer), readBuffer, sizeof(readBuffer));
	if (status != 0)
	{
		lblStatus->Caption = "I2C write/read failed!";
		return;
	}
	float temperature = BufToTemperature(readBuffer);

	lblStatus->Caption = "Read OK";
	AnsiString str;
	str.sprintf("%.2f C", temperature);
	edTemperature->Text = str;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CLm75::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (chbAutoRead->Checked)
		Read();
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

