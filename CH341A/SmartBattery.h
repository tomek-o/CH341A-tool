//---------------------------------------------------------------------------

#ifndef SmartBatteryH
#define SmartBatteryH
//---------------------------------------------------------------------------

namespace SmartBattery
{

enum
{
	I2C_ADDRESS = 0x0B
};

enum Command
{
	CMD_MANUFACTURER_ACCESS = 0x00,			///< optional, implementation specific
	CMD_REMAINING_CAPACITY_ALARM = 0x01,	///< set/get the Low Capacity alarm threshold value
	CMD_REMAINING_TIME_ALARM = 0x02, 		///< set/gets the Remaining Time alarm value

	CMD_BATTERY_MODE = 0x03,				///< selects the various battery operational modes and reports the battery’s capabilities, modes, and flags minor conditions requiring attention

	CMD_AT_RATE = 0x04,						///< first half of a two-function call-set used to set the AtRate value used in calculations made by the AtRateTimeToFull(), AtRateTimeToEmpty(), and AtRateOK() functions
	CMD_AT_RATE_TIME_TO_FULL = 0x05,
	CMD_AT_RATE_TIME_TO_EMPTY = 0x06,
	CMD_AT_RATE_OK = 0x07,

	CMD_TEMPERATURE = 0x08,
	CMD_VOLTAGE = 0x09,
	CMD_CURRENT = 0x0A,
	CMD_AVERAGE_CURRENT = 0x0B,				///< one-minute rolling average based on the current being supplied (or accepted) through the battery's terminals (mA)
	CMD_MAX_ERROR = 0x0C, 					///< Returns the expected margin of error (%) in the state of charge calculation. For example, when MaxError() returns 10% and RelativeStateOfCharge() returns 50%, the Relative StateOfCharge() is actually between 50 and 60%.

	CMD_RELATIVE_STATE_OF_CHARGE = 0x0D,	///< predicted remaining battery capacity expressed as a percentage of FullChargeCapacity() (%)
	CMD_ABSOLUTE_STATE_OF_CHARGE = 0x0E,	///< predicted remaining battery capacity expressed as a percentage of DesignCapacity() (%)
	CMD_REMAINING_CAPACITY = 0x0F,			///< predicted remaining battery capacity expressed in either current (mAh at a C/5 discharge rate) or power (10mWh at a P/5 discharge rate) depending on the setting of the BatteryMode()'s CAPACITY_MODE bit
	CMD_FULL_CHARGE_CAPACITY = 0x10,		///< predicted pack capacity when it is fully charged in either current (mAh at a C/5 discharge rate) or power (10mWh at a P/5 discharge rate) depending on the setting of the BatteryMode()'s CAPACITY_MODE bit
	CMD_RUN_TIME_TO_EMPTY = 0x11,			///< predicted remaining battery life at the present rate of discharge (minutes), calculated based on either current or power depending on the setting of the BatteryMode()'s CAPACITY_MODE bit
	CMD_AVERAGE_TIME_TO_EMPTY = 0x12,		///< 1 minute rolling average of the predicted remaining battery life (minutes), calculated based on either current or power depending on the setting of the BatteryMode()'s CAPACITY_MODE bit
	CMD_AVERAGE_TIME_TO_FULL = 0x13,		///< 1 minute rolling average of the predicted remaining time until the battery reaches full charge (minutes)
	CMD_CHARGING_CURRENT = 0x14,
	CMD_CHARGING_VOLTAGE = 0x15,
	CMD_BATTERY_STATUS = 0x16,				///< Alarm and Status bit flags
	CMD_CYCLE_COUNT = 0x17,					///< number of cycles (approximately equal to the value of DesignCapacity) the battery has experienced
	CMD_DESIGN_CAPACITY = 0x18,				///< theoretical capacity of a new pack, expressed in either current (mAh at a C/5 discharge rate) or power (10mWh at a P/5 discharge rate) depending on the setting of the BatteryMode()'s CAPACITY_MODE bit
	CMD_DESIGN_VOLTAGE = 0x19,				///< theoretical voltage of a new pack (mV)
	CMD_SPEC_INFO = 0x1A,					///< version number of the SB specification the battery pack supports, as well as voltage and current and capacity scaling information in a packed unsigned integer
	CMD_MFG_DATE = 0x1B,					///< returns the date the cell pack was manufactured in a packed integer: (year-1980) * 512 + month * 32 + day
	CMD_SERIAL_NUM = 0x1C,					///< return a serial number; combined with the ManufacturerName(), the DeviceName(), and the ManufactureDate() will uniquely identify the battery (unsigned int)
	// RESERVED              0x1D - 0x1F
	CMD_MANUFACTURER_NAME = 0x20,			///< string
	CMD_DEVICE_NAME = 0x21,					///< string
	CMD_CELL_CHEMISTRY = 0x22,				///< string
	CMD_MANUFACTURER_DATA = 0x23,			///< proprietary data
	// RESERVED_2              0x25 - 0x2E

	CMD_PACK_STATUS = 0x2F,					///< OptionalMfgFunction5, Block (size 11) for bq8011

	// RESERVED_3              0x30 - 0x3B

	CMD_CELL4_VOLTAGE = 0x3C,   			// r/w word - OptionalMfgFunction4 - Individual cell voltages don't work on most (Sony, Lenovo and Dell) packs
	CMD_CELL3_VOLTAGE = 0x3D,   			// r/w word
	CMD_CELL2_VOLTAGE = 0x3E,   			// r/w word
	CMD_CELL1_VOLTAGE = 0x3F,   			// r/w word

	CMD_STATE_OF_HEALTH = 0x4F,				// byte, percents
};

enum BatteryModeBits
{
	MODE_INTERNAL_CHARGE_CONTROLLER = 0x0001,	// R
	MODE_PRIMARY_BATTERY_SUPPORT	= 0x0002,	// R; ability to act as primary/secondary battery in a system
	MODE_CONDITION_REQUEST			= 0x0008,	// R; Conditioning Cycle Requested
	MODE_CHARGE_CONTROLLER_ENABLED	= 0x0100,	// R/W; Internal Charge Control Enabled
	MODE_PRIMARY_BATTERY			= 0x0200,	// R/W; 0 = secondary role, 1 = primary role
	MODE_ALARM						= 0x2000,	// R/W; enable/disable AlarmWarning broadcast to Host and Smart Battery Charger
	MODE_CHARGER					= 0x4000,	// R/W; enable/disable broadcasts of ChargingVoltage and ChargingCurrent to Smart Battery Charger
	MODE_CAPACITY					= 0x8000	// R/W; 0 = report in 1 mA/mAh (default), 1 = report in 10 mA/mAh units
};

enum BatteryStatusBits
{
	STATUS_OVER_CHARGED_ALARM		= 0x8000,
	STATUS_TERMINATE_CHARGE_ALARM	= 0x4000,
	STATUS_OVER_TEMP_ALARM			= 0x1000,
	STATUS_TERMINATE_DISCHARGE_ALARM	= 0x0800,
	STATUS_REMAINING_CAPACITY_ALARM	= 0x0200,
	STATUS_REMAINING_TIME_ALARM		= 0x0100,
	STATUS_INITIALIZED 				= 0x0080,
	STATUS_DISCHARGING				= 0x0040,
	STATUS_FULLY_CHARGED			= 0x0020,
	STATUS_FULLY_DISCHARGED			= 0x0010
};


}

#endif
