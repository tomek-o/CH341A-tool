//---------------------------------------------------------------------------

#ifndef SmartBatteryH
#define SmartBatteryH
//---------------------------------------------------------------------------

namespace SmartBattery
{

enum
{
	ADDRESS = 0x0B
};

enum Command
{
	CMD_MANUFACTURER_ACCESS = 0x00,
	CMD_REMAINING_CAPACITY_ALARM = 0x01,
	CMD_REMAINING_TIME_ALARM = 0x02,

	CMD_BATTERY_MODE = 0x03,

	CMD_AT_RATE = 0x04,
	CMD_AT_RATE_TIME_TO_FULL = 0x05,
	CMD_AT_RATE_TIME_TO_EMPTY = 0x06,
	CMD_AT_RATE_OK = 0x07,

	CMD_TEMPERATURE = 0x08,
	CMD_VOLTAGE = 0x09,
	CMD_CURRENT = 0x0A,
	CMD_AVERAGE_CURRENT = 0x0B, // last minute
	CMD_MAX_ERROR = 0x0C, 		// byte, charge calculation

	CMD_RELATIVE_SOC = 0x0D,	// byte, relative charge
	CMD_ABSOLUTE_SOC = 0x0E,	// byte
	CMD_REMAINING_CAPACITY = 0x0F,
	CMD_FULL_CHARGE_CAPACITY = 0x10,
	CMD_RUN_TIME_TO_EMPTY = 0x11,
	CMD_AVERAGE_TIME_TO_EMPTY = 0x12,
	CMD_TIME_TO_FULL = 0x13,
	CMD_CHARGING_CURRENT = 0x14,
	CMD_CHARGING_VOLTAGE = 0x15,
	CMD_BATTERY_STATUS = 0x16,
	CMD_CYCLE_COUNT = 0x17,
	CMD_DESIGN_CAPACITY = 0x18,
	CMD_DESIGN_VOLTAGE = 0x19,
	CMD_SPEC_INFO = 0x1A,
	CMD_MFG_DATE = 0x1B,
	CMD_SERIAL_NUM = 0x1C,
	// RESERVED              0x1D - 0x1F
	CMD_MFG_NAME = 0x20,   // string
	CMD_DEV_NAME = 0x21,   // string
	CMD_CELL_CHEM = 0x22,  // string
	CMD_MANUFACTURER_DATA = 0x23,
	// RESERVED_2              0x25 - 0x2E

	CMD_PACK_STATUS = 0x2F,   // r/w Word - OptionalMfgFunction5, Block (size 11) for bq8011

	// RESERVED_3              0x30 - 0x3B

	CMD_CELL4_VOLTAGE = 0x3C,   // r/w word - OptionalMfgFunction4 - Individual cell voltages don't work on most (Sony, Lenovo and Dell) packs
	CMD_CELL3_VOLTAGE = 0x3D,   // r/w word
	CMD_CELL2_VOLTAGE = 0x3E,   // r/w word
	CMD_CELL1_VOLTAGE = 0x3F,   // r/w word

	CMD_STATE_OF_HEALTH = 0x4F,   // byte, percents
};

}

#endif
