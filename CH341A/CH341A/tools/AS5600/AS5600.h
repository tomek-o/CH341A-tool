//---------------------------------------------------------------------------

#ifndef AS5600H
#define AS5600H
//---------------------------------------------------------------------------

#include <stdint.h>

namespace AS5600
{
	const uint8_t AS5600_ADDRESS    = 0x36;
	const uint8_t AS5600L_ADDRESS   = 0x40;

	const float RAW_ANGLE_TO_DEGREES     = 360.0f / 4096.0f;

	int read(uint8_t address, uint16_t &rawAngle, uint16_t &angle, bool &magnetDetected, bool &magnetTooStrong, bool &magnetTooWeak);
};

#endif
