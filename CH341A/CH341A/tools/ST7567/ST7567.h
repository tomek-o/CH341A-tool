//---------------------------------------------------------------------------

#ifndef ST7567H
#define ST7567H
//---------------------------------------------------------------------------

#include <stdint.h>

class ST7567
{
public:
	enum Command
	{
		CMD_DISPLAY_OFF = 0xAE,
		CMD_DISPLAY_ON = 0xAF,
		CMD_SET_DISP_START_LINE = 0x40,
		CMD_SET_PAGE = 0xB0,
		CMD_SET_COLUMN_UPPER = 0x10,
		CMD_SET_COLUMN_LOWER = 0x00,
		CMD_SET_ADC_NORMAL = 0xA0,
		CMD_SET_ADC_REVERSE = 0xA1,
		CMD_SET_ALLPTS_NORMAL = 0xA4,
		CMD_SET_ALLPTS_ON = 0xA5,
		CMD_SET_BIAS_9 = 0xA2,
		CMD_SET_BIAS_7 = 0xA3,
		CMD_RMW = 0xE0,
		CMD_RMW_CLEAR = 0xEE,
		CMD_INTERNAL_RESET = 0xE2,
		CMD_SET_COM_NORMAL = 0xC0,
		CMD_SET_COM_REVERSE = 0xC8,
		CMD_SET_POWER_CONTROL = 0x28,
		CMD_SET_RESISTOR_RATIO = 0x20,
		CMD_SET_VOLUME_FIRST = 0x81,
		CMD_SET_VOLUME_SECOND = 0,
		CMD_SET_STATIC_OFF = 0xAC,
		CMD_SET_STATIC_ON = 0xAD,
		CMD_SET_STATIC_REG = 0x0,
		CMD_SET_BOOSTER_FIRST = 0xF8,
		CMD_SET_BOOSTER_234 = 0,
		CMD_SET_BOOSTER_5 = 1,
		CMD_SET_BOOSTER_6 = 3,
		CMD_NOP = 0xE3,
		CMD_TEST = 0xF0,
	};
	ST7567(void);
	int Init(void);
	int SendCmd(int cmd);
	int SendCmdSequence(uint8_t *cmd, unsigned int count);
	void Write (unsigned int line, unsigned int column, char c);
	void Clear(void);

private:
	bool dataCmdState;
	bool resetState;
	void UpdateGpio(void);
};

extern ST7567 st7567;

#endif
