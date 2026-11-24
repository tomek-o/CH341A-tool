//---------------------------------------------------------------------------

#include "Ds18b20.h"
#include <string.h>
#include <mem.h>

//---------------------------------------------------------------------------

namespace
{
const uint8_t CMD_CONVERT_T = 0x44;
const uint8_t CMD_WRITE_SCRATCHPAD = 0x4E;
const uint8_t CMD_READ_SCRATCHPAD = 0xBE;
const uint8_t CMD_COPY_SCRATCHPAD = 0x48;
}

int Ds18b20::ReadScratchpad(const OneWireRom *rom, uint8_t *scratchpad)
{
	int status = ow.SelectRom(rom);
	if (status != CH341OneWire::OK)
		return status;
	status = ow.WriteByte(CMD_READ_SCRATCHPAD);
	if (status != CH341OneWire::OK)
		return status;
	status = ow.ReadBytes(scratchpad, 9);
	if (status != CH341OneWire::OK)
		return status;
	if (CH341OneWire::Crc8(scratchpad, 8) != scratchpad[8])
		return CH341OneWire::ERR_CRC;
	return CH341OneWire::OK;
}

void Ds18b20::Decode(const uint8_t *scratchpad, uint8_t family, Ds18b20Result &result)
{
	memcpy(result.scratchpad, scratchpad, 9);
	result.family = family;
	int16_t raw = static_cast<int16_t>(static_cast<uint16_t>(scratchpad[0] | (scratchpad[1] << 8)));
	if (family == 0x10)
	{
		// DS18S20: 0.5 C steps, COUNT_REMAIN and COUNT_PER_C give extended resolution
		result.resolutionBits = 9;
		const int countRemain = scratchpad[6];
		const int countPerC = scratchpad[7];
		const int truncated = raw >> 1;	// drop the 0.5 C bit (arithmetic shift: floor)
		if (countPerC != 0)
			result.temperature = truncated - 0.25 + static_cast<double>(countPerC - countRemain) / countPerC;
		else
			result.temperature = raw / 2.0;
		result.powerOnValue = (raw == 0x00AA);	// 85 C
	}
	else
	{
		result.resolutionBits = 9 + ((scratchpad[4] >> 5) & 3);
		// undefined low bits at lower resolutions
		raw = static_cast<int16_t>(raw & ~((1 << (12 - result.resolutionBits)) - 1));
		result.temperature = raw / 16.0;
		result.powerOnValue = (raw == 0x0550);	// 85 C
	}
}

int Ds18b20::ReadTemperature(const OneWireRom *rom, uint8_t familyHint, bool parasite, Ds18b20Result &result)
{
	uint8_t family = rom ? rom->id[0] : familyHint;

	int status = ow.SelectRom(rom);
	if (status != CH341OneWire::OK)
		return status;
	status = ow.WriteByte(CMD_CONVERT_T);
	if (status != CH341OneWire::OK)
		return status;

	if (parasite)
	{
		status = ow.StrongPullup(Ds18b20::CONVERSION_TIMEOUT_MS * 8 / 10);	// 800 ms
		if (status != CH341OneWire::OK)
			return status;
	}
	else
	{
		// the device answers read slots with 0 while converting and with 1 when done
		bool done = false;
		for (unsigned int i = 0; i < CONVERSION_TIMEOUT_MS / 5 && !done; i++)
		{
			status = ow.ReadBit(done);
			if (status != CH341OneWire::OK)
				return status;
			if (!done)
				CH341OneWire::SleepMs(5);
		}
		if (!done)
			return CH341OneWire::ERR_TIMEOUT;
	}

	uint8_t scratchpad[9];
	status = ReadScratchpad(rom, scratchpad);
	if (status != CH341OneWire::OK)
		return status;
	Decode(scratchpad, family, result);
	return CH341OneWire::OK;
}

int Ds18b20::SetResolution(const OneWireRom *rom, unsigned int bits, bool store)
{
	if (bits < 9 || bits > 12)
		return CH341OneWire::ERR_PARAM;
	uint8_t scratchpad[9];
	int status = ReadScratchpad(rom, scratchpad);
	if (status != CH341OneWire::OK)
		return status;

	const uint8_t config = static_cast<uint8_t>(((bits - 9) << 5) | 0x1F);
	const uint8_t data[4] = { CMD_WRITE_SCRATCHPAD, scratchpad[2], scratchpad[3], config };
	status = ow.SelectRom(rom);
	if (status != CH341OneWire::OK)
		return status;
	status = ow.WriteBytes(data, sizeof(data));
	if (status != CH341OneWire::OK)
		return status;

	// verify
	status = ReadScratchpad(rom, scratchpad);
	if (status != CH341OneWire::OK)
		return status;
	if (scratchpad[4] != config)
		return CH341OneWire::ERR_CRC;	// write did not take effect: treat like a bus error

	if (store)
	{
		status = ow.SelectRom(rom);
		if (status != CH341OneWire::OK)
			return status;
		status = ow.WriteByte(CMD_COPY_SCRATCHPAD);
		if (status != CH341OneWire::OK)
			return status;
		status = ow.StrongPullup(15);	// EEPROM write takes up to 10 ms and needs strong pull-up on parasite power
		if (status != CH341OneWire::OK)
			return status;
	}
	return CH341OneWire::OK;
}
