#include "CH1115.h"
#include "CH341A.h"
#include <algorithm>
#include <windows.h>

#ifdef __BORLANDC__
#pragma warn -8071
#endif

namespace
{

enum CMD {
	CMD_CONTRAST = 0x81,
	CMD_IREF = 0x82,				///< IREF Resistor Set and Adjust ISEG by PAD: (Double Bytes Command). IREF can be controlled by external resister or internal resister.
	CMD_DISPLAYALL_ON_RESUME = 0xA4,
	CMD_DISPLAYALL_ON = 0xA5,
	CMD_INVERTED_OFF = 0xA6,
	CMD_INVERTED_ON = 0xA7,
	CMD_DISPLAY_OFF = 0xAE,
	CMD_DISPLAY_ON = 0xAF,
	CMD_SET_PAGE_ADDRESS = 0xB0,			///< 0xB0...0xB7
	CMD_SET_COMMON_OUTPUT_SCAN_DIR = 0xC0,	///< 0xC0...0xC8
	CMD_DISPLAYOFFSET = 0xD3,
	CMD_COMPINS = 0xDA,
	CMD_VCOMDETECT = 0xDB,
	CMD_DISPLAYCLOCKDIV = 0xD5,
	CMD_PRECHARGE = 0xD9,
	CMD_MULTIPLEX = 0xA8,
	CMD_LOWCOLUMN = 0x00,
	CMD_HIGHCOLUMN = 0x10,
	CMD_STARTLINE = 0x40,
	CMD_MEMORYMODE = 0x20,
	CMD_MEMORYMODE_HORZONTAL = 0x00,
	CMD_MEMORYMODE_VERTICAL = 0x01,
	CMD_MEMORYMODE_PAGE = 0x10,
	CMD_COLUMNADDR = 0x21,
	CMD_PAGEADDR = 0x22,
	CMD_BREATHING_LIGHT = 0x23,
	CMD_COM_REMAP_OFF = 0xC0,
	CMD_COM_REMAP_ON = 0xC8,
	CMD_CLUMN_REMAP_OFF = 0xA0,
	CMD_CLUMN_REMAP_ON = 0xA1,
	CMD_SEG_HW_CONFIG = 0xA2,
	CMD_CHARGEPUMP = 0x8D,
};

}


CH1115::CH1115(CH341A &ch341a, uint8_t Address):
	ch341a(ch341a)
{
	// Set class instanced variables
	this->address = Address;

	// display is not inverted by default
	this->inverted = false;

	// https://goldenmorninglcd.com/oled-display/0.5-inch-oled-88x48-ch1115-gme8848-01/
	cmd(CMD_DISPLAY_OFF); /*display off*/
	cmd(CMD_LOWCOLUMN); /*set lower column address*/
	cmd(CMD_HIGHCOLUMN); /*set higher column address*/
	cmd(CMD_STARTLINE); /*set display start line*/
	cmd(CMD_SET_PAGE_ADDRESS); /*set page address*/
	cmd(CMD_CONTRAST); /*contract control*/
	cmd(0x80); /*128*/
	//cmd(CMD_IREF);
	//cmd(0x00); /* */
	//cmd(CMD_BREATHING_LIGHT);
	//cmd(0x01); /* */
	cmd(CMD_CLUMN_REMAP_OFF); /*set segment remap*/
	//cmd(CMD_SEG_HW_CONFIG);
	cmd(CMD_DISPLAYALL_ON_RESUME);
	cmd(CMD_INVERTED_OFF); /*normal / reverse*/
	cmd(CMD_MULTIPLEX); /*multiplex ratio*/
	cmd(HEIGHT-1); /*0x2F = 47*/
	cmd(CMD_SET_COMMON_OUTPUT_SCAN_DIR); /*Com scan direction*/
	cmd(CMD_DISPLAYOFFSET); /*set display offset*/
	cmd(0x38);
	cmd(0xD5); /*set osc division*/
	cmd(0x50);
	cmd(0xD9); /*set pre-charge period*/
	cmd(0x22);
	cmd(0xdb); /*set vcomh*/
	cmd(0x35);
	cmd(0xAd); /*set charge pump enable*/
	cmd(0x8B);
	cmd(0x33); /*9v*/
	cmd(0xAF); /*display ON*/

	// clear the buffer and send it to the display
	// if not done display shows garbage data
	this->clear();
}

void CH1115::setOrientation(bool orientation) {
	// remap columns and rows scan direction, effectively flipping the image on display
	if (orientation) {
		this->cmd(CMD_CLUMN_REMAP_OFF);
		this->cmd(CMD_COM_REMAP_OFF);
	} else {
		this->cmd(CMD_CLUMN_REMAP_ON);
		this->cmd(CMD_COM_REMAP_ON);
	}
}


void CH1115::invertDisplay() {
	this->cmd(CMD_INVERTED_OFF | !this->inverted);
	inverted = !inverted;
}

void CH1115::cmd(unsigned char command) {
	uint8_t data[3] = {0x00, 0x00, command};
	data[0] = address << 1;
	ch341a.I2CWriteRead(data, sizeof(data), NULL, 0);
}

void CH1115::data(unsigned char val) {
	uint8_t data[3] = {0x00, CMD_STARTLINE, val};
	data[0] = address << 1;
	ch341a.I2CWriteRead(data, sizeof(data), NULL, 0);
}

void CH1115::data(const unsigned char *buf, unsigned int count) {
	ch341a.I2CWriteCommandWriteBytes(address, CMD_STARTLINE, buf, count);
}


void CH1115::setColumn(unsigned char column)
{
	cmd(0x10|(column>>4));
	cmd(0x00|(column&0x0f));
}

void CH1115::setPage(unsigned char page)
{
	cmd(0xb0+page);
}

void CH1115::clear(void)
{
	uint8_t zeros[WIDTH];
	memset(zeros, 0, sizeof(zeros));
	for(int j=0;j<HEIGHT/8;j++)
	{
		setPage(j);
		setColumn(0);
		data(zeros, sizeof(zeros));
	}
}


void CH1115::setContrast(unsigned char contrast) {
	this->cmd(CMD_CONTRAST);
	this->cmd(contrast);
}

void CH1115::turnOff() {
	this->cmd(CMD_DISPLAY_OFF);
}

void CH1115::turnOn() {
	this->cmd(CMD_DISPLAY_ON);
}


