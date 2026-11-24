//---------------------------------------------------------------------------

#include "OneWire.h"
#include "CH341A.h"
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

//---------------------------------------------------------------------------

namespace
{

// stream commands, see mCH341A_CMD_UIO_STM_* in CH341DLL.H
const uint8_t CMD_IN = 0x00;
const uint8_t CMD_DIR = 0x40;
const uint8_t CMD_OUT = 0x80;
const uint8_t CMD_US = 0xC0;

const unsigned int MAX_CMD = 30;	// CH341A::UIO_MAX_COMMANDS

// slot timing in stream delay units (about 1 us each)
const unsigned int WRITE1_LOW = 4;			// write 1: low 1..15 us
const unsigned int WRITE0_LOW = 63;			// write 0: low 60..120 us
const unsigned int WRITE0_RECOVERY = 5;
const unsigned int SLOT_REST = 63;			// rest of a write-1 slot
const unsigned int READ_REST = 50;			// rest of a read slot (>= 60 us in total)
const unsigned int RESET_PULSES = 8;		// 8 * ~66 us = ~530 us low (480 us minimum)
const unsigned int PRESENCE_SAMPLE = 62;	// presence pulse is surely low between 60 and 75 us after release

inline uint8_t Us(unsigned int units)
{
	if (units < 1)
		units = 1;		// never 0: a zero delay is a long delay
	if (units > 63)
		units = 63;
	return static_cast<uint8_t>(CMD_US | units);
}

}

//---------------------------------------------------------------------------

CH341OneWire::CH341OneWire(void):
	pin(0),
	readLow(2),
	readWait(6)
{
}

void CH341OneWire::SetPin(unsigned int pin)
{
	if (pin > 5)
		pin = 5;
	this->pin = pin;
}

void CH341OneWire::SetReadTiming(unsigned int low, unsigned int wait)
{
	if (low < 1)
		low = 1;
	if (low > 40)
		low = 40;
	if (wait < 1)
		wait = 1;
	if (wait > 40)
		wait = 40;
	readLow = low;
	readWait = wait;
}

int CH341OneWire::Stream(const uint8_t *commands, unsigned int count, unsigned int expectedIn, uint8_t *in)
{
	if (ch341a.UioStream(commands, count, expectedIn, in) != 0)
		return ERR_TRANSPORT;
	return OK;
}

int CH341OneWire::Release(void)
{
	const uint8_t cmd[] = { static_cast<uint8_t>(CMD_OUT | 0), static_cast<uint8_t>(CMD_DIR | 0) };
	return Stream(cmd, sizeof(cmd), 0, NULL);
}

int CH341OneWire::IdleLevel(bool &high)
{
	const uint8_t cmd[] = { static_cast<uint8_t>(CMD_OUT | 0), static_cast<uint8_t>(CMD_DIR | 0), CMD_IN };
	uint8_t in = 0;
	int status = Stream(cmd, sizeof(cmd), 1, &in);
	if (status == OK)
		high = (in & Mask()) != 0;
	return status;
}

int CH341OneWire::Reset(bool &present)
{
	std::vector<uint8_t> cmd;
	cmd.push_back(CMD_OUT | 0);				// latch low, direction decides
	cmd.push_back(CMD_DIR | Mask());		// pull low
	for (unsigned int i = 0; i < RESET_PULSES; i++)
		cmd.push_back(Us(63));
	cmd.push_back(CMD_DIR | 0);				// release
	cmd.push_back(Us(PRESENCE_SAMPLE));
	cmd.push_back(CMD_IN);					// presence sample
	for (unsigned int i = 0; i < RESET_PULSES; i++)
		cmd.push_back(Us(63));
	cmd.push_back(CMD_IN);					// line must be high again
	uint8_t in[2] = {0};
	int status = Stream(&cmd[0], static_cast<unsigned int>(cmd.size()), 2, in);
	if (status != OK)
		return status;
	present = (in[0] & Mask()) == 0;
	if ((in[1] & Mask()) == 0)
	{
		present = false;
		return ERR_BUS_STUCK;
	}
	return OK;
}

int CH341OneWire::RunSlots(const uint8_t *slots, unsigned int count, uint8_t *bitsOut)
{
	const uint8_t mask = Mask();
	std::vector<uint8_t> cmd;
	unsigned int reads = 0;		// read slots in the current packet
	unsigned int readsDone = 0;	// read slots already stored in bitsOut
	cmd.push_back(CMD_OUT | 0);
	for (unsigned int i = 0; i <= count; i++)
	{
		uint8_t slot[6];
		unsigned int len = 0;
		if (i < count)
		{
			switch (slots[i])
			{
			case SlotWrite1:
				slot[len++] = CMD_DIR | mask;
				slot[len++] = Us(WRITE1_LOW);
				slot[len++] = CMD_DIR | 0;
				slot[len++] = Us(SLOT_REST);
				break;
			case SlotWrite0:
				slot[len++] = CMD_DIR | mask;
				slot[len++] = Us(WRITE0_LOW);
				slot[len++] = CMD_DIR | 0;
				slot[len++] = Us(WRITE0_RECOVERY);
				break;
			default:
				slot[len++] = CMD_DIR | mask;
				slot[len++] = Us(readLow);
				slot[len++] = CMD_DIR | 0;
				slot[len++] = Us(readWait);
				slot[len++] = CMD_IN;
				slot[len++] = Us(READ_REST);
				break;
			}
		}
		// send the packet if the next slot does not fit (or at the end); a slot never spans two packets
		if (i == count || cmd.size() + len > MAX_CMD)
		{
			if (cmd.size() > 1)
			{
				uint8_t in[CH341A::UIO_MAX_COMMANDS];
				int status = Stream(&cmd[0], static_cast<unsigned int>(cmd.size()), reads, in);
				if (status != OK)
					return status;
				for (unsigned int j = 0; j < reads; j++)
					bitsOut[readsDone++] = static_cast<uint8_t>((in[j] & mask) ? 1 : 0);
			}
			cmd.clear();
			cmd.push_back(CMD_OUT | 0);
			reads = 0;
		}
		for (unsigned int j = 0; j < len; j++)
			cmd.push_back(slot[j]);
		if (len == 6)
			reads++;
	}
	return OK;
}

int CH341OneWire::WriteBytes(const uint8_t *data, unsigned int count)
{
	std::vector<uint8_t> slots;
	for (unsigned int i = 0; i < count; i++)
		for (unsigned int bit = 0; bit < 8; bit++)
			slots.push_back(((data[i] >> bit) & 1) ? SlotWrite1 : SlotWrite0);
	if (slots.empty())
		return OK;
	return RunSlots(&slots[0], static_cast<unsigned int>(slots.size()), NULL);
}

int CH341OneWire::WriteByte(uint8_t value)
{
	return WriteBytes(&value, 1);
}

int CH341OneWire::ReadBytes(uint8_t *data, unsigned int count)
{
	if (count == 0)
		return OK;
	std::vector<uint8_t> slots(count * 8, static_cast<uint8_t>(SlotRead));
	std::vector<uint8_t> bits(count * 8);
	int status = RunSlots(&slots[0], static_cast<unsigned int>(slots.size()), &bits[0]);
	if (status != OK)
		return status;
	for (unsigned int i = 0; i < count; i++)
	{
		data[i] = 0;
		for (unsigned int bit = 0; bit < 8; bit++)
			if (bits[i * 8 + bit])
				data[i] |= static_cast<uint8_t>(1u << bit);
	}
	return OK;
}

int CH341OneWire::ReadBit(bool &bit)
{
	const uint8_t slot = SlotRead;
	uint8_t value = 0;
	int status = RunSlots(&slot, 1, &value);
	if (status == OK)
		bit = (value != 0);
	return status;
}

int CH341OneWire::SelectRom(const OneWireRom *rom)
{
	bool present = false;
	int status = Reset(present);
	if (status != OK)
		return status;
	if (!present)
		return ERR_NO_PRESENCE;
	if (rom == NULL)
		return WriteByte(0xCC);
	status = WriteByte(0x55);
	if (status != OK)
		return status;
	return WriteBytes(rom->id, 8);
}

int CH341OneWire::ReadRom(OneWireRom &rom)
{
	bool present = false;
	int status = Reset(present);
	if (status != OK)
		return status;
	if (!present)
		return ERR_NO_PRESENCE;
	status = WriteByte(0x33);
	if (status != OK)
		return status;
	status = ReadBytes(rom.id, 8);
	if (status != OK)
		return status;
	if (Crc8(rom.id, 7) != rom.id[7])
		return ERR_CRC;
	return OK;
}

int CH341OneWire::Search(std::vector<OneWireRom> &roms, unsigned int maxDevices)
{
	roms.clear();
	int lastDiscrepancy = 0;
	bool lastDevice = false;
	OneWireRom rom;
	memset(&rom, 0, sizeof(rom));
	while (!lastDevice && roms.size() < maxDevices)
	{
		bool present = false;
		int status = Reset(present);
		if (status != OK)
			return status;
		if (!present)
			return ERR_NO_PRESENCE;
		status = WriteByte(0xF0);
		if (status != OK)
			return status;

		int lastZero = 0;
		int pendingDir = -1;	// direction bit of the previous position, sent together with the next two reads
		for (int bitNumber = 1; bitNumber <= 64; bitNumber++)
		{
			uint8_t slots[3];
			unsigned int n = 0;
			if (pendingDir >= 0)
				slots[n++] = pendingDir ? SlotWrite1 : SlotWrite0;
			slots[n++] = SlotRead;	// id bit
			slots[n++] = SlotRead;	// complement of the id bit
			uint8_t bits[2] = {0};
			status = RunSlots(slots, n, bits);
			if (status != OK)
				return status;
			int idBit = bits[0];
			int cmpIdBit = bits[1];
			if (idBit && cmpIdBit)	// nobody is answering
				return roms.empty() ? ERR_NO_PRESENCE : OK;
			int direction;
			const unsigned int byteNumber = static_cast<unsigned int>((bitNumber - 1) / 8);
			const uint8_t byteMask = static_cast<uint8_t>(1u << ((bitNumber - 1) % 8));
			if (idBit != cmpIdBit)
			{
				direction = idBit;
			}
			else
			{
				if (bitNumber < lastDiscrepancy)
					direction = (rom.id[byteNumber] & byteMask) ? 1 : 0;
				else
					direction = (bitNumber == lastDiscrepancy) ? 1 : 0;
				if (direction == 0)
					lastZero = bitNumber;
			}
			if (direction)
				rom.id[byteNumber] |= byteMask;
			else
				rom.id[byteNumber] &= static_cast<uint8_t>(~byteMask);
			pendingDir = direction;
		}
		// the direction of the last bit has to be sent as well
		{
			uint8_t slot = pendingDir ? SlotWrite1 : SlotWrite0;
			status = RunSlots(&slot, 1, NULL);
			if (status != OK)
				return status;
		}
		if (Crc8(rom.id, 7) != rom.id[7])
			return ERR_CRC;
		roms.push_back(rom);
		lastDiscrepancy = lastZero;
		if (lastDiscrepancy == 0)
			lastDevice = true;
	}
	return roms.empty() ? ERR_NO_PRESENCE : OK;
}

int CH341OneWire::StrongPullup(unsigned int ms)
{
	// latch high first, then make it an output: no low glitch
	const uint8_t on[] = { static_cast<uint8_t>(CMD_OUT | Mask()), static_cast<uint8_t>(CMD_DIR | Mask()) };
	int status = Stream(on, sizeof(on), 0, NULL);
	if (status != OK)
		return status;
	SleepMs(ms);
	return Release();
}

uint8_t CH341OneWire::Crc8(const uint8_t *data, unsigned int count)
{
	uint8_t crc = 0;
	for (unsigned int i = 0; i < count; i++)
	{
		uint8_t b = data[i];
		for (int bit = 0; bit < 8; bit++)
		{
			uint8_t mix = static_cast<uint8_t>((crc ^ b) & 1);
			crc >>= 1;
			if (mix)
				crc ^= 0x8C;
			b >>= 1;
		}
	}
	return crc;
}

const char* CH341OneWire::ErrorText(int code)
{
	switch (code)
	{
	case OK:
		return "OK";
	case ERR_TRANSPORT:
		return "CH341 is not opened or USB transfer failed";
	case ERR_NO_PRESENCE:
		return "No presence pulse: no device answering";
	case ERR_CRC:
		return "CRC error (check wiring, pull-up and read timing)";
	case ERR_BUS_STUCK:
		return "DQ is held low after reset: short circuit, missing pull-up, or wrong pin";
	case ERR_TIMEOUT:
		return "Timeout waiting for the device";
	case ERR_PARAM:
		return "Invalid parameter";
	default:
		return "Unknown error";
	}
}

const char* CH341OneWire::FamilyName(uint8_t family)
{
	switch (family)
	{
	case 0x01:
		return "DS1990A / DS2401 serial number";
	case 0x02:
		return "DS1991 iButton";
	case 0x04:
		return "DS1994 iButton with clock";
	case 0x05:
		return "DS2405 switch";
	case 0x10:
		return "DS18S20 / DS1820 temperature sensor";
	case 0x12:
		return "DS2406 switch";
	case 0x1D:
		return "DS2423 counter";
	case 0x22:
		return "DS1822 temperature sensor";
	case 0x23:
		return "DS2433 EEPROM";
	case 0x26:
		return "DS2438 battery monitor";
	case 0x28:
		return "DS18B20 temperature sensor";
	case 0x29:
		return "DS2408 8-channel switch";
	case 0x3A:
		return "DS2413 dual switch";
	case 0x3B:
		return "DS1825 / MAX31850 temperature sensor";
	case 0x42:
		return "DS28EA00 temperature sensor";
	default:
		return "unknown family";
	}
}

void CH341OneWire::SleepMs(unsigned int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(ms * 1000u);
#endif
}
