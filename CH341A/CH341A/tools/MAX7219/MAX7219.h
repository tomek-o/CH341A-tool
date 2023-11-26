//---------------------------------------------------------------------------

#ifndef MAX7219H
#define MAX7219H
//---------------------------------------------------------------------------

#include <stdint.h>

namespace MAX7219
{
	enum { DIGITS = 8 };
	/**
		\param intensity 0x00...0x0F, duty cycle 1/32...31/32 for MAX7219, 1/16...15/16 for MAX7221
		\param digitCount multiplexing; setting this to too low might make segment current (set by resistor) too high
	*/
	int init(uint8_t intensity, uint8_t digitCount);
	/**
		\param intensity 0x00...0x0F
	*/
	int setIntensity(uint8_t intensity);
	/**	\brief Write data to all (DIGITS = 8) digit registers

	Bit order for 8x8 LED matrix:
		- first digit byte = bottom row
		- LBS of each digit byte = first column
	*/
	int setDigits(const uint8_t *digits);
};

#endif
