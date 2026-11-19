//---------------------------------------------------------------------------
/** \file
    SD / SDHC / MMC card access in SPI mode on top of CH341A. No GUI code here.
    Pins: CS = D0 (CS0), SCK = D3, MOSI = D5, MISO = D7 (see SD_WIRING.txt).
    Every command exchange is a single CH341StreamSPI4 call (CS low for the whole call).
*/

#ifndef SdCardH
#define SdCardH
//---------------------------------------------------------------------------

#include <stdint.h>
#include <string>
#include <vector>
#include "MemProg.h"

namespace SdMmc
{

using namespace MemProg;

/** Receives a line of text for every command / response, implemented by UI */
class SdTrace
{
public:
	virtual ~SdTrace(void) {}
	virtual void Trace(const std::string &line) = 0;
};

/** Response format expected after a command */
enum SdResponse
{
	SdR1 = 0,	///< 1 byte
	SdR2,		///< 2 bytes (CMD13)
	SdR3R7		///< 1 + 4 bytes (CMD58, CMD8)
};

struct SdCid
{
	uint8_t raw[16];
	uint8_t manufacturer;
	char oem[3];
	char product[6];
	unsigned int revMajor, revMinor;
	uint32_t serial;
	unsigned int year, month;
	bool crcOk;
};

struct SdCsd
{
	uint8_t raw[16];
	unsigned int version;		///< 1 (SDSC) or 2 (SDHC/SDXC), 0 if unknown
	uint64_t capacityBytes;
	unsigned int readBlockLen;
};

class SdCard
{
public:
	enum { SECTOR_SIZE = 512 };

	SdCard(void): trace(NULL), initialized(false), highCapacity(false), version2(false), ocr(0) {}

	void SetTrace(SdTrace *t) { trace = t; }
	SdTrace* GetTrace(void) const { return trace; }
	bool IsInitialized(void) const { return initialized; }
	bool IsHighCapacity(void) const { return highCapacity; }
	bool IsVersion2(void) const { return version2; }
	uint32_t GetOcr(void) const { return ocr; }

	/** 80 clocks with CS high, CMD0, CMD8, ACMD41 loop, CMD58, CMD16. Fills the trace with every step. */
	int Init(Progress *progress);

	/** Sends one command, \a extra receives bytes after R1 (R3/R7: 4 bytes, R2: 1 byte) */
	int Command(uint8_t cmd, uint32_t arg, SdResponse type, uint8_t &r1, uint8_t *extra);
	/** CMD55 + command */
	int AppCommand(uint8_t cmd, uint32_t arg, SdResponse type, uint8_t &r1, uint8_t *extra);

	int ReadCid(SdCid &cid);
	int ReadCsd(SdCsd &csd);
	/** CMD58, updates GetOcr() */
	int ReadOcr(uint32_t &value);
	/** CMD13: R1 and the second status byte */
	int ReadStatus(uint8_t &r1, uint8_t &r2);

	/** Reads \a count sectors starting at \a sector (CMD17 for each) */
	int ReadSectors(uint32_t sector, uint32_t count, uint8_t *data, Progress *progress);
	/** Writes \a count sectors (CMD24 for each) */
	int WriteSectors(uint32_t sector, uint32_t count, const uint8_t *data, Progress *progress);

	static std::string DescribeR1(uint8_t r1);
	static std::string DescribeOcr(uint32_t ocr);
	static std::string DescribeCid(const SdCid &cid);
	static std::string DescribeCsd(const SdCsd &csd);

private:
	SdTrace *trace;
	bool initialized;
	bool highCapacity;
	bool version2;
	uint32_t ocr;

	void Log(const std::string &line);
	/** Command (with arg) then a data block of \a length bytes (+ 2 CRC) read in the same CS window */
	int CommandReadBlock(uint8_t cmd, uint32_t arg, uint8_t *data, unsigned int length);
	uint32_t Address(uint32_t sector) const { return highCapacity ? sector : sector * SECTOR_SIZE; }
};

/** The card shared by the SD windows (initialized state, addressing mode) */
extern SdCard sdCard;

/** Short connection instructions shown at the top of SD windows */
const char* GetConnectionText(void);

}	// namespace SdMmc

#endif
