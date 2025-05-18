//---------------------------------------------------------------------------
#pragma hdrstop

#include "ST7567.h"
#include "CH341A.h"
#include "SSD1306/textRenderer/8x8_font.h"
#include <vector>

//---------------------------------------------------------------------------

#pragma package(smart_init)

/*
Based on https://github.com/dquadros/LCD12864/blob/main/PiPico/BasicTest/BasicTest.ino

MIT License

Copyright (c) 2024 Daniel Quadros

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

/*
Connect CH341A SPI SCK, MOSI and CS0 to ST7567.
Connect CH341A UART TX to ST7567 DC (data/command). DC = H -> data, L -> command.
Connect CH341A UART RX to ST7567 RESET.
*/

ST7567 st7567;

namespace
{

enum
{
	PIN_DC = 8,		// = TXD, output only
	PIN_RESET = 9,	// RXD
};

enum
{
	LCD_WIDTH = 128,
	LCD_HEIGHT = 64
};

// Display initialization commands
byte cmdInit[] =
{
	// cmd                                  // delay
	ST7567::CMD_SET_BIAS_7,                 0, 
	ST7567::CMD_SET_ADC_NORMAL,             0,
	ST7567::CMD_SET_COM_NORMAL,             0,
	ST7567::CMD_SET_DISP_START_LINE,        0,
	ST7567::CMD_SET_POWER_CONTROL | 0x4,    50,
	ST7567::CMD_SET_POWER_CONTROL | 0x6,    50,
	ST7567::CMD_SET_POWER_CONTROL | 0x7,    10,
	ST7567::CMD_SET_RESISTOR_RATIO | 0x6,   0,
	ST7567::CMD_DISPLAY_ON,                 0,
	ST7567::CMD_SET_ALLPTS_NORMAL,          0,
	ST7567::CMD_SET_VOLUME_FIRST,           0,
	ST7567::CMD_SET_VOLUME_SECOND | 13,     0	// 13 = contrast value
};

#if 0
// Digits-only (0 to 9) font
byte gc[][7] =
{
	 {0x7C, 0x86, 0x8A, 0x92, 0xA2, 0xC2, 0x7C}  // 0
	,{0x02, 0x02, 0x42, 0xFE, 0x02, 0x02, 0x02}  // 1
	,{0x42, 0x86, 0x8A, 0x92, 0xA2, 0x42, 0x02}  // 2
	,{0x44, 0x82, 0x92, 0x92, 0x92, 0x92, 0x6C}  // 3
	,{0x08, 0x18, 0x28, 0x48, 0x88, 0xFE, 0x0A}  // 4
	,{0xE4, 0xA2, 0xA2, 0xA2, 0xA2, 0x92, 0x8C}  // 5
	,{0x3C, 0x52, 0x92, 0x92, 0x92, 0x92, 0x4C}  // 6
	,{0x82, 0x84, 0x88, 0x90, 0xA0, 0xC0, 0x80}  // 7
	,{0x6C, 0x92, 0x92, 0x92, 0x92, 0x92, 0x6C}  // 8
	,{0x64, 0x92, 0x92, 0x92, 0x92, 0x94, 0x78}  // 9
};
#endif

#ifdef __BORLANDC__
#pragma warn -8071
#endif
uint8_t reverse(uint8_t b) {
   b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
   b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
   b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
   return b;
}
#ifdef __BORLANDC__
#pragma warn .8071
#endif

}


ST7567::ST7567(void):
	dataCmdState(false),
	resetState(false)
{

}

void ST7567::UpdateGpio(void)
{
	uint32_t direction = (1u << PIN_DC) | (1u << PIN_RESET);

	uint32_t value = 0;
	if (dataCmdState)
		value |= (1u << PIN_DC);
	if (resetState)
		value |= (1u << PIN_RESET);

	ch341a.SetGpioOutputs(direction, value);
}

int ST7567::Init(void)
{
	// reset controller (active low)
	resetState = false;
	UpdateGpio();
	Sleep(500);
	resetState = true;
	UpdateGpio();
	Sleep(100);

	// Configure controller and clean the screen
	SendCmdSequence(cmdInit, sizeof(cmdInit)/2);
	Clear();
	return 0;
}

// Clear display
void ST7567::Clear(void)
{
	// write zeroes in all columns in all pages
	for (uint8_t p = 0; p < LCD_HEIGHT/8; p++)
	{
		SendCmd(CMD_SET_PAGE | p);
		SendCmd(CMD_SET_COLUMN_UPPER | 0);
        SendCmd(CMD_SET_COLUMN_LOWER | 0);
		dataCmdState = true;
		UpdateGpio();
		std::vector<uint8_t> data;
		data.resize(LCD_WIDTH);
		memset(&data[0], 0, data.size());
		ch341a.SpiTransfer(&data[0], data.size());
	}
}

// Writes a char at line l (0 to 7), colunm  c (0 to 15)
void ST7567::Write (unsigned int line, unsigned int column, char c)

{
	if (line >= LCD_HEIGHT / 8)
		return;
	if (column >= LCD_WIDTH / 8)
		return;
		
	uint8_t font_width = font_8x8[0];
	uint8_t font_height = font_8x8[1];
#ifdef __BORLANDC__
#pragma warn -8071
#endif
	uint16_t seek = (c - 32) * (font_width * font_height) / 8 + 2;
#ifdef __BORLANDC__
#pragma warn .8071
#endif
	if (seek >= sizeof(font_8x8) - 2)
		seek = 2;
	const byte *pc = font_8x8 + seek;
	uint8_t buf[7];
	for (int i=0; i<7; i++)
	{
    	buf[i] = reverse(pc[i]);
	}
#if 0
	byte *pc = gc[charId];
	uint8_t buf[7];
	memcpy(buf, pc, sizeof(buf));
#endif

#ifdef __BORLANDC__
#pragma warn -8071
#endif
	column = column << 3; // 8 bytes per char
#ifdef __BORLANDC__
#pragma warn .8071
#endif

	SendCmd(CMD_SET_PAGE | (7-line));  // page numbered from bottom to top
	SendCmd(CMD_SET_COLUMN_UPPER | (column >> 4));
    SendCmd(CMD_SET_COLUMN_LOWER | (column & 0x0F));

	dataCmdState = true;
	UpdateGpio();

	ch341a.SpiTransfer(buf, sizeof(buf));
}

// Send a sequence of commands to the display, with delays
int ST7567::SendCmdSequence(uint8_t *cmd, unsigned int count)
{
	dataCmdState = false;
	UpdateGpio();

	uint8_t buf[1];

	for (unsigned int i = 0; i < count*2; i+=2)
	{
		buf[0] = cmd[i];
		ch341a.SpiTransfer(buf, sizeof(buf));
		if (cmd[i+1] != 0)
		{
			Sleep(cmd[i+1]);
		}
	}
	return 0;
}

// Send a command byte to the display
int ST7567::SendCmd (int cmd)
{
	uint8_t val = static_cast<uint8_t>(cmd);
	dataCmdState = false;
	UpdateGpio();

	return ch341a.SpiTransfer(&val, sizeof(val));
}


