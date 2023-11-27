//---------------------------------------------------------------------------


#pragma hdrstop

#include "AS5600.h"
#include "CH341A.h"
#include "Log.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

// configuration
const uint8_t REG_ZMCO = 0x00;				// 8 b
const uint8_t REG_ZPOS = 0x01;				// 16b, +0x02
const uint8_t REG_MPOS = 0x03;				// 16b
const uint8_t REG_MANG = 0x05;				// 16b
const uint8_t REG_CONF = 0x07;				// 16b

const uint8_t CONF_POWER_MODE    = 0x03;
const uint8_t CONF_HYSTERESIS    = 0x0C;
const uint8_t CONF_OUTPUT_MODE   = 0x30;
const uint8_t CONF_PWM_FREQUENCY = 0xC0;
const uint8_t CONF_SLOW_FILTER   = 0x03;
const uint8_t CONF_FAST_FILTER   = 0x1C;
const uint8_t CONF_WATCH_DOG     = 0x20;

// output
const uint8_t REG_RAW_ANGLE = 0x0C;			// 16b, +0x0D
const uint8_t REG_ANGLE     = 0x0E;			// 16b, +0x0F

// AS5600L (only) I2C address
const uint8_t REG_I2CADDR   = 0x20;
const uint8_t REG_I2CUPDT   = 0x21;

// status
const uint8_t REG_STATUS    = 0x0B;
const uint8_t REG_AGC       = 0x1A;
const uint8_t REG_MAGNITUDE = 0x1B;			// 16b, +0x1C
const uint8_t REG_BURN      = 0xFF;

const uint8_t STATUS_MAGNET_HIGH   = 0x08;
const uint8_t STATUS_MAGNET_LOW    = 0x10;
const uint8_t STATUS_MAGNET_DETECT = 0x20;


uint16_t read16(uint8_t address, uint8_t reg)
{
	int16_t data = 0xFFFF;
	int status = ch341a.I2CWriteCommandReadWord(address, reg, data);
	(void)status;
	// first of the register = most significant bits
	const uint8_t *ptr = reinterpret_cast<uint8_t*>(&data);
	uint16_t val = static_cast<uint16_t>(256 * ptr[0] + ptr[1]);
	//LOG("AS5600 reg 0x%02X = 0x%04X\n", static_cast<unsigned int>(reg), static_cast<unsigned int>(val));
	return val;
}

uint8_t read8(uint8_t address, uint8_t reg)
{
	uint8_t data = 0xFF;
	int status = ch341a.I2CWriteCommandReadByte(address, reg, data);
	(void)status;
	return data;
}

}	// namespace


int AS5600::read(uint8_t address, uint16_t &rawAngle, uint16_t &angle, bool &magnetDetected, bool &magnetTooStrong, bool &magnetTooWeak)
{
	(void)RAW_ANGLE_TO_DEGREES;

	rawAngle = static_cast<uint16_t>(read16(address, REG_RAW_ANGLE) & 0x0FFF);
	angle = static_cast<uint16_t>(read16(address, REG_ANGLE) & 0x0FFF);

	uint8_t statusReg = read8(address, REG_STATUS);
	magnetDetected = statusReg & STATUS_MAGNET_DETECT;
	magnetTooStrong = statusReg & STATUS_MAGNET_HIGH;
	magnetTooWeak = statusReg & STATUS_MAGNET_LOW;

	return 0;
}
