//---------------------------------------------------------------------------

#ifndef Ds18b20H
#define Ds18b20H
//---------------------------------------------------------------------------

#include "OneWire.h"

/** \file
	DS18B20 (family 0x28), DS1822 (0x22) and DS18S20 / DS1820 (0x10) temperature sensors
	on top of CH341OneWire.
*/

struct Ds18b20Result
{
	uint8_t scratchpad[9];
	uint8_t family;				///< 0 if the family is not known (Skip ROM used)
	int resolutionBits;			///< 9..12; the 0x10 family always gives 9 bits (+ extended resolution calculation)
	double temperature;			///< degrees C
	bool powerOnValue;			///< reads 85.0 C: the power-on value, conversion may not have happened
};

class Ds18b20
{
public:
	Ds18b20(CH341OneWire &ow): ow(ow) {
	}

	/** Starts a conversion and reads the temperature.
		\param rom device ROM (Match ROM) or NULL for a single device on the bus (Skip ROM)
		\param familyHint family code used when \a rom is NULL: 0x10 selects the DS18S20 calculation,
			anything else the DS18B20 one
		\param parasite true if the device is parasite powered: DQ is driven high during the conversion
		\return 0 or CH341OneWire::ErrorCode
	*/
	int ReadTemperature(const OneWireRom *rom, uint8_t familyHint, bool parasite, Ds18b20Result &result);

	/** Writes the resolution (9..12 bits, DS18B20 / DS1822 only), keeps the alarm bytes TH and TL.
		\param store also copy the scratchpad to EEPROM (survives power cycle)
	*/
	int SetResolution(const OneWireRom *rom, unsigned int bits, bool store);

	/** Decodes a scratchpad (CRC is not checked here) */
	static void Decode(const uint8_t *scratchpad, uint8_t family, Ds18b20Result &result);

	enum { CONVERSION_TIMEOUT_MS = 1000 };	// worst case is 750 ms

private:
	CH341OneWire &ow;
	int ReadScratchpad(const OneWireRom *rom, uint8_t *scratchpad);
};

#endif
