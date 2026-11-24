//---------------------------------------------------------------------------

#ifndef OneWireH
#define OneWireH
//---------------------------------------------------------------------------

#include <stdint.h>
#include <vector>

/** \file
	1-Wire master using one of the CH341A D5-D0 pins (CH341 "UIO stream" commands).

	The CH341 is not a 1-Wire master, so open drain is emulated: the output latch of the DQ pin is
	held low and only the pin DIRECTION is changed (direction = output -> DQ pulled low, direction = input
	-> DQ released and pulled up by the external resistor). Pulse widths are generated inside the CH341
	by the stream "microsecond delay" commands, so USB latency does not matter as long as one slot
	is sent in one stream packet (a stream holds at most CH341A::UIO_MAX_COMMANDS command bytes).

	Notes:
	- every stream sets the direction of ALL of D5-D0: other pins are left as inputs
	- a stream delay of 0 is NOT "no delay" (it measured ~260 us), so delay values below 1 are never used
	- timing: one delay unit is about 1 us, one IN command takes about 2 us (measured, see notes)
*/

struct OneWireRom
{
	uint8_t id[8];	///< family code, 6 bytes serial number, CRC
};

class CH341OneWire
{
public:
	enum ErrorCode
	{
		OK = 0,
		ERR_TRANSPORT = -1,		///< CH341 not opened / USB error
		ERR_NO_PRESENCE = -10,	///< no presence pulse after reset
		ERR_CRC = -11,
		ERR_BUS_STUCK = -12,	///< DQ stays low after reset (short circuit, or a device holding the line)
		ERR_TIMEOUT = -13,		///< device did not finish (e.g. temperature conversion)
		ERR_PARAM = -14
	};

	CH341OneWire(void);

	/** DQ pin: 0..5 = D0..D5 (D0 = CS0, D3 = SCK, D5 = MOSI on the programmer's SPI pins) */
	void SetPin(unsigned int pin);
	unsigned int GetPin(void) const {
		return pin;
	}
	/** Read slot timing in stream delay units (about 1 us each): low time and wait before sampling.
		The device data must be sampled within 15 us after the falling edge. Values are limited to 1..40.
	*/
	void SetReadTiming(unsigned int low, unsigned int wait);
	unsigned int GetReadLow(void) const {
		return readLow;
	}
	unsigned int GetReadWait(void) const {
		return readWait;
	}

	/** Releases DQ (all of D5-D0 become inputs) */
	int Release(void);
	/** Reads DQ level without driving it; \a high is false if the line is stuck low or has no pull-up */
	int IdleLevel(bool &high);
	/** Reset pulse + presence detect. \return 0, ERR_BUS_STUCK or a transport error; \a present set on 0 */
	int Reset(bool &present);

	int WriteByte(uint8_t value);
	int WriteBytes(const uint8_t *data, unsigned int count);
	int ReadBytes(uint8_t *data, unsigned int count);
	int ReadBit(bool &bit);

	/** Reset + Read ROM (0x33): only for a single device on the bus */
	int ReadRom(OneWireRom &rom);
	/** Reset + Skip ROM (0xCC) if \a rom is NULL, otherwise Reset + Match ROM (0x55) + ROM */
	int SelectRom(const OneWireRom *rom);
	/** Search ROM (0xF0) algorithm; \return 0 if at least one device was found */
	int Search(std::vector<OneWireRom> &roms, unsigned int maxDevices = 32);

	/** Drives DQ high (push-pull) for \a ms milliseconds, e.g. for parasite-powered devices
		during temperature conversion, then releases it. Blocking.
	*/
	int StrongPullup(unsigned int ms);

	static uint8_t Crc8(const uint8_t *data, unsigned int count);
	static const char* ErrorText(int code);
	static const char* FamilyName(uint8_t family);
	static void SleepMs(unsigned int ms);

private:
	enum Slot
	{
		SlotWrite0 = 0,
		SlotWrite1 = 1,
		SlotRead = 2
	};
	unsigned int pin;
	unsigned int readLow;
	unsigned int readWait;

	uint8_t Mask(void) const {
		return static_cast<uint8_t>(1u << pin);
	}
	/** Sends one stream (split into several packets, never inside a slot), returns the sampled
		bits of read slots in \a bitsOut (one entry per read slot)
	*/
	int RunSlots(const uint8_t *slots, unsigned int count, uint8_t *bitsOut);
	int Stream(const uint8_t *commands, unsigned int count, unsigned int expectedIn, uint8_t *in);
};

#endif
