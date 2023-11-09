//---------------------------------------------------------------------------

#ifndef HD44780H
#define HD44780H
//---------------------------------------------------------------------------

namespace HD44780
{
	enum Command
	{
		CMD_CLEARDISPLAY = 0x01,
		CMD_RETURNHOME = 0x02,
		CMD_ENTRYMODESET = 0x04,
		CMD_DISPLAYCONTROL = 0x08,
		CMD_CURDISPSHIFT = 0x10,
		CMD_FUNCTIONSET = 0x20,
		CMD_SETCGRAMADDR = 0x40,
		CMD_SETDDRAMADDR = 0x80,
	};

	enum EntryMode
	{
		ENTRY_MODE_AUTOSHIFT = 0x01,
		ENTRY_MODE_LEFT2RIGHT = 0x02,
	};

	enum Control
	{
		CONTROL_DISPLAYON = 0x04,
		CONTROL_CURSORON = 0x02,
		CONTROL_BLINKON = 0x01,
	};

	enum DisplayShift
	{
		DISPSHIFT_DISPLAYMOVE = 0x08,
		DISPSHIFT_CURSORMOVE = 0x00,
		DISPSHIFT_MOVERIGHT = 0x04,
		DISPSHIFT_MOVELEFT = 0x00,
	};

	enum Function
	{
		FUNCTION_8BITMODE = 0x10,
		FUNCTION_4BITMODE = 0x00,
		FUNCTION_2LINE = 0x08,
		FUNCTION_1LINE = 0x00,
		FUNCTION_5x10DOTS = 0x04,
		FUNCTION_5x8DOTS  = 0x00,
	};
}

#endif
