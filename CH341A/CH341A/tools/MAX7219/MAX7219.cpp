//---------------------------------------------------------------------------


#pragma hdrstop

#include "MAX7219.h"
#include "Log.h"
#include "CH341A.h"
#include <vector>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

enum MAX7219_REG
{
	MAX7219_REG_NOOP = 0x00,	// used for cascading displays
	MAX7219_DIGIT0 = 0x01,
	MAX7219_DIGIT1 = 0x02,
	MAX7219_DIGIT2 = 0x03,
	MAX7219_DIGIT3 = 0x04,
	MAX7219_DIGIT4 = 0x05,
	MAX7219_DIGIT5 = 0x06,
	MAX7219_DIGIT6 = 0x07,
	MAX7219_DIGIT7 = 0x08,
	MAX7219_DECODE_MODE = 0x09,
	MAX7219_INTENSITY = 0x0A,
	MAX7219_SCAN_LIMIT = 0x0B,
	MAX7219_SHUTDOWN = 0x0C,
	MAX7219_DISPLAY_TEST = 0x0F	// overrides all others control registers, turning LEDs on
};

int writeReg(enum MAX7219_REG reg, uint8_t val)
{
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = val;
	return ch341a.SpiTransfer(buf, sizeof(buf));
}

int writeRegCascade(enum MAX7219_REG reg, uint8_t val, unsigned int cascadeCount)
{
	std::vector<uint8_t> buf;
	buf.resize(cascadeCount * 2);
	for (unsigned int i=0; i<cascadeCount; i++)
	{
		buf[i*2] = reg;
		buf[(i*2)+1] = val;
	}
	return ch341a.SpiTransfer(&buf[0], buf.size());
}

}


int MAX7219::init(uint8_t intensity, uint8_t digitCount)
{
	int err = 0;

	for (int reg = MAX7219_DIGIT0; reg <= MAX7219_DIGIT7; reg++)
	{
    	err |= writeReg(static_cast<MAX7219_REG>(reg), 0x00);
	}

	err |= writeReg(MAX7219_INTENSITY, intensity);

	err |= writeReg(MAX7219_SCAN_LIMIT, static_cast<uint8_t>(digitCount-1));
	err |= writeReg(MAX7219_DECODE_MODE, 0x00);	// no code-B decoding for digits
	err |= writeReg(MAX7219_DISPLAY_TEST, 0x00);
	err |= writeReg(MAX7219_SHUTDOWN, 0x01);

#if 0
	uint8_t digits[DIGITS] = {
		0x05,
		0x05 << 1,
		0x05 << 2,
		0x05 << 3,
		0x05 << 4,
		0x05 << 5,
		0x05 << 6,
		0x05 << 7,
	};
	err |= setDigits(digits);
#endif

	return err;
}

int MAX7219::setIntensity(uint8_t intensity)
{
	int err = 0;
	err |= writeReg(MAX7219_INTENSITY, intensity);
	return err;	
}

int MAX7219::setDigits(const uint8_t *digits)
{
#if 1
	int err = 0;
	for (unsigned int i=0; i<DIGITS; i++)
	{
		err |= writeReg(static_cast<enum MAX7219_REG>(MAX7219_DIGIT0 + i), digits[i]);
	}
	return err;
#else
	uint8_t buffer[DIGITS*2];
	for (unsigned int i=0; i<DIGITS; i++)
	{
		buffer[i*2] = static_cast<uint8_t>(MAX7219_DIGIT0 + i);
		buffer[(i*2)+1] = digits[i];
	}
	return ch341a.SpiTransfer(buffer, sizeof(buffer));
#endif
}

int MAX7219::initCascade(uint8_t intensity, uint8_t digitCount, unsigned int cascadeCount)
{
	int err = 0;

	for (int reg = MAX7219_DIGIT0; reg <= MAX7219_DIGIT7; reg++)
	{
		err |= writeRegCascade(static_cast<MAX7219_REG>(reg), 0x00, cascadeCount);
	}

	err |= writeRegCascade(MAX7219_INTENSITY, intensity, cascadeCount);

	err |= writeRegCascade(MAX7219_SCAN_LIMIT, static_cast<uint8_t>(digitCount-1), cascadeCount);
	err |= writeRegCascade(MAX7219_DECODE_MODE, 0x00, cascadeCount);	// no code-B decoding for digits
	err |= writeRegCascade(MAX7219_DISPLAY_TEST, 0x00, cascadeCount);
	err |= writeRegCascade(MAX7219_SHUTDOWN, 0x01, cascadeCount);
	return err;
}

int MAX7219::setIntensityCascade(uint8_t intensity, unsigned int cascadeCount)
{
	int err = 0;
	err |= writeRegCascade(MAX7219_INTENSITY, intensity, cascadeCount);
	return err;
}

int MAX7219::setDigitCascade(unsigned int digitId, const uint8_t *digits, unsigned int cascadeCount)
{
	std::vector<uint8_t> buf;
	buf.resize(cascadeCount * 2);
	for (unsigned int i=0; i<cascadeCount; i++)
	{
		buf[i*2] = static_cast<uint8_t>(MAX7219_DIGIT0 + digitId);
		buf[(i*2)+1] = digits[i];
	}
	return ch341a.SpiTransfer(&buf[0], buf.size());
}

