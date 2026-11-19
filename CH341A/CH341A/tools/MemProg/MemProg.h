//---------------------------------------------------------------------------
/** \file
    EEPROM / FLASH access on top of CH341A: SPI flash (25xx series) and
    I2C EEPROM (24xx series). No GUI code here.
*/

#ifndef MemProgH
#define MemProgH
//---------------------------------------------------------------------------

#include <stdint.h>
#include <vector>

namespace MemProg
{

/** Progress/cancel hook, implemented by UI */
class Progress
{
public:
	virtual ~Progress(void) {}
	/** Called regularly, \a done / \a total in bytes (or steps) */
	virtual void Report(unsigned long done, unsigned long total) = 0;
	/** Return true to abort current operation */
	virtual bool IsCancelled(void) = 0;
};

enum Status
{
	StatusOk = 0,
	StatusNotOpened = -1,
	StatusCommError = -2,
	StatusCancelled = -3,
	StatusTimeout = -4,
	StatusNoDevice = -5,
	StatusVerifyFailed = -6,
	StatusInvalidArgs = -7,
	StatusWrongDevice = -8
};

const char* StatusText(int status);

//---------------------------------------------------------------------------

struct SpiFlashId
{
	uint8_t manufacturer;
	uint8_t memType;
	uint8_t capacity;

	bool IsValid(void) const;
	/** Size in bytes assumed from capacity byte (2^capacity), 0 if unknown */
	unsigned long GuessSize(void) const;
	const char* ManufacturerName(void) const;
};

/** One erase type of the SFDP table */
struct SfdpEraseType
{
	unsigned long size;	///< bytes, 0 if the type is not used
	uint8_t opcode;
};

/** What a chip tells about itself in its SFDP (JESD216) basic flash parameter table */
struct SfdpInfo
{
	unsigned int major, minor;	///< SFDP revision
	unsigned long size;		///< bytes
	unsigned int pageSize;		///< bytes, 0 if the table is too old to tell
	unsigned int addrMode;		///< 0: 3-byte address only, 1: 3 or 4 bytes, 2: 4 bytes only
	bool has4kErase;
	uint8_t erase4kOpcode;
	SfdpEraseType eraseTypes[4];

	SfdpInfo(void): major(0), minor(0), size(0), pageSize(0), addrMode(0), has4kErase(false), erase4kOpcode(0)
	{
		for (int i=0; i<4; i++)
		{
			eraseTypes[i].size = 0;
			eraseTypes[i].opcode = 0;
		}
	}
};

/** SPI flash, 25xx series (JEDEC compatible), uses CS0.
    Also used for SPI EEPROM (25xxx/95xxx): same bus and command set, only address length differs.
*/
class SpiFlash
{
public:
	enum { PAGE_SIZE_DEFAULT = 256 };
	enum { SECTOR_SIZE = 4096 };

	enum ProgramMode
	{
		ProgramPage = 0,	///< regular page program (0x02)
		ProgramAaiWord,		///< SST auto address increment, word (0xAD)
		ProgramAaiByte		///< SST auto address increment, byte (0xAF)
	};

	SpiFlash(void): size(0), pageSize(PAGE_SIZE_DEFAULT), addrBytes(0), programMode(ProgramPage), lastErasedSectors(0), lastProgrammedPages(0) {}

	/** \a addrBytesCount: 1..4, or 0 for automatic (3, or 4 for chips over 16 MB).
		1 byte with size of 512 B (e.g. 25x040) carries A8 in the opcode. */
	void Configure(unsigned long sizeBytes, unsigned int pageSizeBytes, unsigned int addrBytesCount = 0);
	unsigned long GetSize(void) const { return size; }
	void SetProgramMode(ProgramMode mode) { programMode = mode; }

	int ReadId(SpiFlashId &id);
	/** Reads SFDP (command 0x5A, always 3-byte address): size, page size and erase commands.
		StatusNoDevice if the chip has no (usable) SFDP. Does not depend on Configure(). */
	int ReadSfdp(SfdpInfo &info);
	int ReadStatus(uint8_t &sr);
	/** Clear block protection bits (SR1 = 0) */
	int Unprotect(void);

	int Read(uint32_t addr, uint8_t *data, unsigned long count, Progress *progress);
	/** Page program; target area must be erased. If \a skipBlank, all-0xFF pages are not written. */
	int Write(uint32_t addr, const uint8_t *data, unsigned long count, bool skipBlank, Progress *progress);
	int EraseChip(Progress *progress);
	/** Erase all 4 KB sectors (opcode 0x20/0x21) touched by [addr, addr + count) */
	int EraseSectors(uint32_t addr, unsigned long count, Progress *progress);
	/** Writes only what differs: reads the chip, then for every 4 KB sector touched by the range
		- identical: skipped;
		- data only clears bits (1 -> 0): differing pages are programmed, no erase;
		- data needs bits set (0 -> 1): sector is erased and its non-blank pages programmed.
		Bytes of a partly covered sector outside the range are preserved.
		\a assumedCurrent: if not NULL, it is taken as the current contents of [addr, addr + count) instead of
		reading them from the chip (caller knows the chip holds that, e.g. it was read earlier); only the
		bytes of partly covered sectors around the range are then read. Uses sector erase (0x20/0x21).
		Counts of what was done are available from GetLastErasedSectors() / GetLastProgrammedPages(). */
	int WriteChanged(uint32_t addr, const uint8_t *data, unsigned long count, Progress *progress,
		const uint8_t *assumedCurrent = NULL);
	unsigned long GetLastErasedSectors(void) const { return lastErasedSectors; }
	unsigned long GetLastProgrammedPages(void) const { return lastProgrammedPages; }
	/** For EEPROMs (no erase command): write 0xFF to whole chip */
	int FillBlank(Progress *progress);
	/** Compare chip contents with \a data; \a mismatchAddr is set on StatusVerifyFailed */
	int Verify(uint32_t addr, const uint8_t *data, unsigned long count, uint32_t &mismatchAddr, Progress *progress);

private:
	unsigned long size;
	unsigned int pageSize;
	unsigned int addrBytes;
	ProgramMode programMode;
	unsigned long lastErasedSectors;
	unsigned long lastProgrammedPages;

	int Command(uint8_t cmd);
	int ReadSfdpBytes(uint32_t addr, uint8_t *data, unsigned int count);
	int WriteEnable(void);
	int WaitReady(unsigned int timeoutMs, Progress *progress);
	unsigned int AddrLen(void) const;
	int WritePages(uint32_t addr, const uint8_t *data, unsigned long count, bool skipBlank, Progress *progress);
	int WriteAai(uint32_t addr, const uint8_t *data, unsigned long count, bool skipBlank, Progress *progress);
	/** fills \a buf with opcode + address, returns bytes used */
	unsigned int BuildHeader(uint8_t *buf, uint8_t opcode3, uint8_t opcode4, uint32_t addr) const;
};

//---------------------------------------------------------------------------

/** Atmel/Adesto DataFlash AT45DBxxx (1 Mbit to 64 Mbit), uses CS0.
    Data is handled as whole pages, including the extra bytes of 264/528/1056 B pages,
    so addresses and sizes have to be multiples of the page size.
*/
class At45
{
public:
	At45(void): pageSize(0), pageCount(0), manufacturer(0), deviceId1(0), deviceId2(0) {}

	/** Reads ID and status register, sets up geometry. StatusNoDevice if no AT45DB responds. */
	int Detect(void);
	unsigned long GetSize(void) const { return static_cast<unsigned long>(pageSize) * pageCount; }
	unsigned int GetPageSize(void) const { return pageSize; }
	unsigned int GetPageCount(void) const { return pageCount; }
	bool IsPowerOfTwoPage(void) const { return pageSize == 256 || pageSize == 512 || pageSize == 1024; }
	void GetId(uint8_t &man, uint8_t &dev1, uint8_t &dev2) const { man = manufacturer; dev1 = deviceId1; dev2 = deviceId2; }

	int ReadStatus(uint8_t &sr);
	/** Disable sector protection (3D 2A 7F 9A) */
	int DisableProtection(void);

	int Read(uint32_t addr, uint8_t *data, unsigned long count, Progress *progress);
	/** Main memory page program through buffer 1 with built-in erase (0x82) */
	int Write(uint32_t addr, const uint8_t *data, unsigned long count, Progress *progress);
	int EraseChip(Progress *progress);
	int Verify(uint32_t addr, const uint8_t *data, unsigned long count, uint32_t &mismatchAddr, Progress *progress);

private:
	unsigned int pageSize;
	unsigned int pageCount;
	uint8_t manufacturer;
	uint8_t deviceId1;
	uint8_t deviceId2;

	bool CheckRange(uint32_t addr, unsigned long count) const;
	unsigned int OffsetBits(void) const;
	int WaitReady(unsigned int timeoutMs, Progress *progress);
};

//---------------------------------------------------------------------------

/** SPI EEPROM (25xxx / 95xxx), to be used with SpiFlash */
struct SpiEepromType
{
	const char *name;
	unsigned long size;		///< bytes
	unsigned int pageSize;	///< bytes
	unsigned int addrBytes;	///< 1..3
};

const SpiEepromType* GetSpiEepromTypes(unsigned int &count);

//---------------------------------------------------------------------------

/** Microwire EEPROM, 93Cxx series.
    Wiring as for SPI: CS = D0, SK = D3, DI = D5 (MOSI), DO = D7 (MISO).
    Data is handled as bytes; in x16 organisation words are stored MSB first (big endian),
    both addresses and sizes must be even then.
*/
struct MicrowireType
{
	const char *name;
	unsigned long size;			///< bytes
	unsigned int addrBitsX16;	///< address bits in command, x16 organisation
	unsigned int addrBitsX8;	///< address bits in command, x8 organisation
};

const MicrowireType* GetMicrowireTypes(unsigned int &count);

class Microwire
{
public:
	Microwire(void): type(0), x16(true) {}

	void Configure(const MicrowireType *mwType, bool wordOrganisation16);
	unsigned long GetSize(void) const;

	int Read(uint32_t addr, uint8_t *data, unsigned long count, Progress *progress);
	int Write(uint32_t addr, const uint8_t *data, unsigned long count, Progress *progress);
	/** ERAL command */
	int Erase(Progress *progress);
	int Verify(uint32_t addr, const uint8_t *data, unsigned long count, uint32_t &mismatchAddr, Progress *progress);

private:
	const MicrowireType *type;
	bool x16;

	unsigned int AddrBits(void) const;
	bool CheckRange(uint32_t addr, unsigned long count) const;
	int Transfer(std::vector<uint8_t> &bits);
	int WaitReady(Progress *progress);
	int WriteCommand(unsigned int opcode, unsigned int addrField, bool waitReady, Progress *progress);
};

//---------------------------------------------------------------------------

/** I2C EEPROM, 24Cxx series */
struct I2cEepromType
{
	const char *name;
	unsigned long size;			///< bytes
	unsigned int pageSize;		///< bytes
	unsigned int addrBytes;		///< address bytes sent after device address (1 or 2)
	unsigned int addrBitsInDev;	///< number of high address bits carried in device address
};

const I2cEepromType* GetI2cEepromTypes(unsigned int &count);

/** Result of scanning I2C addresses 0x50..0x57 */
struct I2cScanResult
{
	unsigned int ackMask;	///< bit n set if address 0x50 + n ACKs
	unsigned int blocks;	///< length of the first run of ACKing addresses
	uint8_t baseAddr;		///< first ACKing address (7-bit)
	bool valid;				///< ACKing addresses look like one EEPROM: 1, 2, 4 or 8 aligned addresses
};

class I2cEeprom
{
public:
	I2cEeprom(void): type(0), devAddr(0x50), autoAddress(false) {}

	/** \a autoAddress: ignore \a baseAddr7bit, Probe() takes the address of the device found */
	void Configure(const I2cEepromType *eepromType, uint8_t baseAddr7bit, bool autoAddress = false);
	uint8_t GetAddress(void) const { return devAddr; }

	/** Looks for ACKs on 0x50..0x57. An EEPROM answers on 1 (24C01/02, 24C32 and bigger),
		2 (24C04, 24C1024), 4 (24C08) or 8 (24C16) consecutive addresses. */
	static int Scan(I2cScanResult &result);
	unsigned long GetSize(void) const;

	/** Scans the bus and checks that the device found matches the selected type
		(number of addresses it occupies) and address. StatusNoDevice if nothing ACKs,
		StatusWrongDevice if the ACKing addresses do not match the selected chip type.
		Result of the scan is kept in GetScan(). */
	int Probe(void);
	const I2cScanResult& GetScan(void) const { return scan; }

	int Read(uint32_t addr, uint8_t *data, unsigned long count, Progress *progress);
	/** Page write with ACK polling; \a skipBlank skips all-0xFF chunks (erased chip) */
	int Write(uint32_t addr, const uint8_t *data, unsigned long count, bool skipBlank, Progress *progress);
	/** Fill whole chip with 0xFF */
	int Erase(Progress *progress);
	int Verify(uint32_t addr, const uint8_t *data, unsigned long count, uint32_t &mismatchAddr, Progress *progress);

private:
	const I2cEepromType *type;
	uint8_t devAddr;
	bool autoAddress;
	I2cScanResult scan;

	/** Prepares device address byte (8-bit, write) + address bytes, returns used count */
	unsigned int BuildHeader(uint8_t *buf, uint32_t addr) const;
	uint8_t DeviceAddr7(uint32_t addr) const;
	int WaitWriteDone(uint32_t addr, Progress *progress);
};

}	// namespace MemProg

#endif
