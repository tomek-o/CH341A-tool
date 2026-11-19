//---------------------------------------------------------------------------

#pragma hdrstop

#include "MemProg.h"
#include "CH341A.h"
#include <windows.h>
#include <string.h>
#include <stddef.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace MemProg
{

static const unsigned int SPI_READ_CHUNK = 2048;
static const unsigned int I2C_READ_CHUNK = 256;

const char* StatusText(int status)
{
	switch (status)
	{
	case StatusOk:
		return "OK";
	case StatusNotOpened:
		return "CH341 is not opened";
	case StatusCommError:
		return "CH341 communication error";
	case StatusCancelled:
		return "Cancelled";
	case StatusTimeout:
		return "Timeout (chip busy or not responding)";
	case StatusNoDevice:
		return "No device detected";
	case StatusVerifyFailed:
		return "Verify failed";
	case StatusInvalidArgs:
		return "Invalid arguments";
	case StatusWrongDevice:
		return "Device found, but it does not match the selected chip type";
	default:
		return "Unknown error";
	}
}

static int MapError(int ch341Status)
{
	if (ch341Status == 0)
		return StatusOk;
	if (ch341Status == -1)
		return StatusNotOpened;
	return StatusCommError;
}

static bool IsBlank(const uint8_t *data, unsigned long count)
{
	for (unsigned long i=0; i<count; i++)
	{
		if (data[i] != 0xFF)
			return false;
	}
	return true;
}

static void Report(Progress *progress, unsigned long done, unsigned long total)
{
	if (progress)
		progress->Report(done, total);
}

static bool Cancelled(Progress *progress)
{
	return progress && progress->IsCancelled();
}

//---------------------------------------------------------------------------
// SPI flash
//---------------------------------------------------------------------------

bool SpiFlashId::IsValid(void) const
{
	// floating MISO reads as all 1, shorted/no power as all 0
	if (manufacturer == 0xFF && memType == 0xFF && capacity == 0xFF)
		return false;
	if (manufacturer == 0x00 && memType == 0x00 && capacity == 0x00)
		return false;
	return true;
}

unsigned long SpiFlashId::GuessSize(void) const
{
	if (capacity >= 0x10 && capacity <= 0x1F)
		return 1UL << capacity;
	return 0;
}

const char* SpiFlashId::ManufacturerName(void) const
{
	switch (manufacturer)
	{
	case 0x01: return "Spansion/Cypress";
	case 0x1C: return "EON";
	case 0x1F: return "Adesto/Atmel";
	case 0x20: return "Micron/Numonyx/ST";
	case 0x37: return "AMIC";
	case 0x62: return "SST (Microchip)";
	case 0x7F: return "(continuation code)";
	case 0x85: return "Puya";
	case 0x8C: return "ESMT";
	case 0xA1: return "Fudan";
	case 0xBF: return "SST";
	case 0xC2: return "Macronix";
	case 0xC8: return "GigaDevice";
	case 0xD5: return "Zbit (Zetta)";
	case 0xEF: return "Winbond";
	default:   return "unknown manufacturer";
	}
}

void SpiFlash::Configure(unsigned long sizeBytes, unsigned int pageSizeBytes, unsigned int addrBytesCount)
{
	size = sizeBytes;
	pageSize = pageSizeBytes ? pageSizeBytes : PAGE_SIZE_DEFAULT;
	addrBytes = (addrBytesCount <= 4) ? addrBytesCount : 0;
}

unsigned int SpiFlash::AddrLen(void) const
{
	if (addrBytes != 0)
		return addrBytes;
	return (size > 0x1000000UL) ? 4 : 3;
}

int SpiFlash::Command(uint8_t cmd)
{
	return MapError(ch341a.SpiTransfer(&cmd, 1));
}

int SpiFlash::WriteEnable(void)
{
	return Command(0x06);
}

int SpiFlash::ReadId(SpiFlashId &id)
{
	uint8_t buf[4] = {0x9F, 0xFF, 0xFF, 0xFF};
	int status = MapError(ch341a.SpiTransfer(buf, sizeof(buf)));
	if (status != StatusOk)
		return status;
	id.manufacturer = buf[1];
	id.memType = buf[2];
	id.capacity = buf[3];
	return id.IsValid() ? StatusOk : StatusNoDevice;
}

int SpiFlash::ReadStatus(uint8_t &sr)
{
	uint8_t buf[2] = {0x05, 0xFF};
	int status = MapError(ch341a.SpiTransfer(buf, sizeof(buf)));
	if (status != StatusOk)
		return status;
	sr = buf[1];
	return StatusOk;
}

int SpiFlash::Unprotect(void)
{
	// Old SST parts need "enable write status register" (0x50) instead of WREN, others ignore it
	int status = Command(0x50);
	if (status != StatusOk)
		return status;
	uint8_t buf[2] = {0x01, 0x00};
	status = MapError(ch341a.SpiTransfer(buf, sizeof(buf)));
	if (status != StatusOk)
		return status;
	status = WaitReady(1000, NULL);
	if (status != StatusOk)
		return status;

	status = WriteEnable();
	if (status != StatusOk)
		return status;
	buf[0] = 0x01;
	buf[1] = 0x00;
	status = MapError(ch341a.SpiTransfer(buf, sizeof(buf)));
	if (status != StatusOk)
		return status;
	return WaitReady(1000, NULL);
}

int SpiFlash::WaitReady(unsigned int timeoutMs, Progress *progress)
{
	const DWORD start = GetTickCount();
	for (;;)
	{
		uint8_t sr;
		int status = ReadStatus(sr);
		if (status != StatusOk)
			return status;	// do not treat link failure as "ready"
		if ((sr & 0x01) == 0)
			return StatusOk;
		if (Cancelled(progress))
			return StatusCancelled;
		if ((GetTickCount() - start) > timeoutMs)
			return StatusTimeout;
		Sleep(1);
	}
}

unsigned int SpiFlash::BuildHeader(uint8_t *buf, uint8_t opcode3, uint8_t opcode4, uint32_t addr) const
{
	unsigned int n = 0;
	const unsigned int len = AddrLen();
	uint8_t opcode = (len == 4) ? opcode4 : opcode3;
	if (len == 1 && size == 512)
		opcode |= static_cast<uint8_t>(((addr >> 8) & 1) << 3);	// A8 sits in the opcode
	buf[n++] = opcode;
	for (unsigned int i=len; i>0; i--)
		buf[n++] = static_cast<uint8_t>(addr >> (8 * (i - 1)));
	return n;
}

int SpiFlash::Read(uint32_t addr, uint8_t *data, unsigned long count, Progress *progress)
{
	if (size == 0 || (addr + static_cast<unsigned long>(count)) > size)
		return StatusInvalidArgs;

	std::vector<uint8_t> buf;
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		if (Cancelled(progress))
			return StatusCancelled;

		unsigned int n = static_cast<unsigned int>(count - done);
		if (n > SPI_READ_CHUNK)
			n = SPI_READ_CHUNK;

		buf.assign(5 + n, 0xFF);
		unsigned int hdr = BuildHeader(&buf[0], 0x03, 0x13, addr + done);
		int status = MapError(ch341a.SpiTransfer(&buf[0], hdr + n));
		if (status != StatusOk)
			return status;
		memcpy(data + done, &buf[hdr], n);
		done += n;
		Report(progress, done, count);
	}
	return StatusOk;
}

int SpiFlash::Write(uint32_t addr, const uint8_t *data, unsigned long count, bool skipBlank, Progress *progress)
{
	if (size == 0 || (addr + static_cast<unsigned long>(count)) > size)
		return StatusInvalidArgs;
	if (programMode == ProgramPage)
		return WritePages(addr, data, count, skipBlank, progress);
	return WriteAai(addr, data, count, skipBlank, progress);
}

int SpiFlash::WriteAai(uint32_t addr, const uint8_t *data, unsigned long count, bool skipBlank, Progress *progress)
{
	const unsigned int unit = (programMode == ProgramAaiWord) ? 2 : 1;
	const uint8_t opcode = (programMode == ProgramAaiWord) ? static_cast<uint8_t>(0xAD) : static_cast<uint8_t>(0xAF);
	if ((addr % unit) != 0)
		return StatusInvalidArgs;

	unsigned long done = 0;
	bool inAai = false;
	int status = StatusOk;
	Report(progress, 0, count);
	while (done < count && status == StatusOk)
	{
		if (Cancelled(progress))
		{
			status = StatusCancelled;
			break;
		}

		// odd last byte of word programming is padded with 0xFF
		uint8_t value[2] = {0xFF, 0xFF};
		const unsigned int n = (count - done >= unit) ? unit : static_cast<unsigned int>(count - done);
		memcpy(value, data + done, n);

		if (skipBlank && IsBlank(value, unit))
		{
			if (inAai)
			{
				status = Command(0x04);	// WRDI, leaves AAI mode
				if (status == StatusOk)
					status = WaitReady(100, progress);
				inAai = false;
			}
		}
		else if (!inAai)
		{
			status = WriteEnable();
			if (status == StatusOk)
			{
				uint8_t buf[7];
				unsigned int len = BuildHeader(buf, opcode, opcode, addr + done);
				memcpy(&buf[len], value, unit);
				status = MapError(ch341a.SpiTransfer(buf, len + unit));
			}
			if (status == StatusOk)
				status = WaitReady(100, progress);
			inAai = true;
		}
		else
		{
			uint8_t buf[3];
			buf[0] = opcode;
			memcpy(&buf[1], value, unit);
			status = MapError(ch341a.SpiTransfer(buf, 1 + unit));
			if (status == StatusOk)
				status = WaitReady(100, progress);
		}

		done += unit;
		Report(progress, done < count ? done : count, count);
	}

	if (inAai)
	{
		// always leave AAI mode, also after an error
		const int wrdi = Command(0x04);
		if (status == StatusOk)
			status = wrdi;
		if (status == StatusOk)
			status = WaitReady(100, progress);
	}
	return status;
}

int SpiFlash::WritePages(uint32_t addr, const uint8_t *data, unsigned long count, bool skipBlank, Progress *progress)
{
	std::vector<uint8_t> buf;
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		if (Cancelled(progress))
			return StatusCancelled;

		const uint32_t cur = addr + done;
		unsigned int n = pageSize - (cur % pageSize);	// do not cross page boundary
		if (n > count - done)
			n = static_cast<unsigned int>(count - done);

		if (!(skipBlank && IsBlank(data + done, n)))
		{
			int status = WriteEnable();
			if (status != StatusOk)
				return status;

			buf.resize(5 + n);
			unsigned int hdr = BuildHeader(&buf[0], 0x02, 0x12, cur);
			memcpy(&buf[hdr], data + done, n);
			status = MapError(ch341a.SpiTransfer(&buf[0], hdr + n));
			if (status != StatusOk)
				return status;

			status = WaitReady(1000, progress);
			if (status != StatusOk)
				return status;
		}
		done += n;
		Report(progress, done, count);
	}
	return StatusOk;
}

int SpiFlash::EraseChip(Progress *progress)
{
	int status = WriteEnable();
	if (status != StatusOk)
		return status;
	status = Command(0xC7);
	if (status != StatusOk)
		return status;
	// big chips can need minutes
	return WaitReady(5 * 60 * 1000, progress);
}

int SpiFlash::EraseSectors(uint32_t addr, unsigned long count, Progress *progress)
{
	if (size == 0 || count == 0 || (addr + static_cast<unsigned long>(count)) > size)
		return StatusInvalidArgs;

	const uint32_t first = addr - (addr % SECTOR_SIZE);
	const unsigned long total = (addr + count - first + SECTOR_SIZE - 1) / SECTOR_SIZE;
	unsigned long done = 0;
	Report(progress, 0, total);
	for (uint32_t a = first; a < addr + count; a += SECTOR_SIZE)
	{
		if (Cancelled(progress))
			return StatusCancelled;
		int status = WriteEnable();
		if (status != StatusOk)
			return status;
		uint8_t buf[5];
		unsigned int len = BuildHeader(buf, 0x20, 0x21, a);
		status = MapError(ch341a.SpiTransfer(buf, len));
		if (status != StatusOk)
			return status;
		status = WaitReady(3000, progress);
		if (status != StatusOk)
			return status;
		done++;
		Report(progress, done, total);
	}
	return StatusOk;
}

int SpiFlash::WriteChanged(uint32_t addr, const uint8_t *data, unsigned long count, Progress *progress,
	const uint8_t *assumedCurrent)
{
	lastErasedSectors = 0;
	lastProgrammedPages = 0;
	if (size == 0 || count == 0 || (addr + static_cast<unsigned long>(count)) > size)
		return StatusInvalidArgs;

	// work on whole sectors: the part of a sector outside the range keeps its current contents
	const uint32_t first = addr - (addr % SECTOR_SIZE);
	uint32_t end = addr + static_cast<uint32_t>(count);
	end = ((end + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
	if (end > size)
		end = static_cast<uint32_t>(size);

	std::vector<uint8_t> current(end - first);
	int status = StatusOk;
	if (assumedCurrent == NULL)
	{
		status = Read(first, &current[0], current.size(), progress);
	}
	else
	{
		// only the bytes around the range have to come from the chip
		memcpy(&current[addr - first], assumedCurrent, count);
		if (addr > first)
			status = Read(first, &current[0], addr - first, NULL);
		const uint32_t rangeEnd = addr + static_cast<uint32_t>(count);
		if (status == StatusOk && rangeEnd < end)
			status = Read(rangeEnd, &current[rangeEnd - first], end - rangeEnd, NULL);
	}
	if (status != StatusOk)
		return status;
	std::vector<uint8_t> target(current);
	memcpy(&target[addr - first], data, count);

	const unsigned long sectors = (end - first + SECTOR_SIZE - 1) / SECTOR_SIZE;
	Report(progress, 0, sectors);
	for (unsigned long s=0; s<sectors; s++)
	{
		if (Cancelled(progress))
			return StatusCancelled;

		const uint32_t secOffset = static_cast<uint32_t>(s * SECTOR_SIZE);
		unsigned long secLen = SECTOR_SIZE;
		if (secOffset + secLen > current.size())
			secLen = current.size() - secOffset;
		const uint8_t *cur = &current[secOffset];
		const uint8_t *tgt = &target[secOffset];

		bool differs = false;
		bool needsErase = false;
		for (unsigned long i=0; i<secLen; i++)
		{
			if (cur[i] != tgt[i])
			{
				differs = true;
				if ((cur[i] & tgt[i]) != tgt[i])
				{
					needsErase = true;
					break;
				}
			}
		}

		if (differs)
		{
			const uint32_t secAddr = first + secOffset;
			if (needsErase)
			{
				status = EraseSectors(secAddr, secLen, NULL);
				if (status != StatusOk)
					return status;
				lastErasedSectors++;
			}
			for (unsigned long p=0; p<secLen; p+=pageSize)
			{
				unsigned long n = pageSize;
				if (p + n > secLen)
					n = secLen - p;
				// after an erase the chip holds 0xFF, otherwise compare with what is there
				const bool needed = needsErase ? !IsBlank(tgt + p, n) : (memcmp(cur + p, tgt + p, n) != 0);
				if (!needed)
					continue;
				status = Write(secAddr + p, tgt + p, n, false, NULL);
				if (status != StatusOk)
					return status;
				lastProgrammedPages++;
			}
		}
		Report(progress, s + 1, sectors);
	}
	return StatusOk;
}

int SpiFlash::FillBlank(Progress *progress)
{
	if (size == 0)
		return StatusInvalidArgs;
	std::vector<uint8_t> blank(size, 0xFF);
	return Write(0, &blank[0], blank.size(), false, progress);
}

int SpiFlash::Verify(uint32_t addr, const uint8_t *data, unsigned long count, uint32_t &mismatchAddr, Progress *progress)
{
	if (size == 0 || (addr + static_cast<unsigned long>(count)) > size)
		return StatusInvalidArgs;

	std::vector<uint8_t> chunk;
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		unsigned int n = static_cast<unsigned int>(count - done);
		if (n > SPI_READ_CHUNK)
			n = SPI_READ_CHUNK;
		chunk.resize(n);
		int status = Read(addr + done, &chunk[0], n, NULL);
		if (status != StatusOk)
			return status;
		for (unsigned int i=0; i<n; i++)
		{
			if (chunk[i] != data[done + i])
			{
				mismatchAddr = addr + done + i;
				return StatusVerifyFailed;
			}
		}
		done += n;
		Report(progress, done, count);
		if (Cancelled(progress))
			return StatusCancelled;
	}
	return StatusOk;
}

//---------------------------------------------------------------------------
// AT45 DataFlash
//---------------------------------------------------------------------------

int At45::ReadStatus(uint8_t &sr)
{
	uint8_t buf[2] = {0xD7, 0xFF};
	int status = MapError(ch341a.SpiTransfer(buf, sizeof(buf)));
	if (status != StatusOk)
		return status;
	sr = buf[1];
	return StatusOk;
}

int At45::Detect(void)
{
	pageSize = 0;
	pageCount = 0;

	uint8_t id[4] = {0x9F, 0xFF, 0xFF, 0xFF};
	int status = MapError(ch341a.SpiTransfer(id, sizeof(id)));
	if (status != StatusOk)
		return status;
	manufacturer = id[1];
	deviceId1 = id[2];
	deviceId2 = id[3];

	const unsigned int density = deviceId1 & 0x1F;
	if (manufacturer != 0x1F || (deviceId1 & 0xE0) != 0x20)
		return StatusNoDevice;
	if (density < 2 || density > 8)	// 1 Mbit .. 64 Mbit, larger ones need 4 address bytes
		return StatusInvalidArgs;

	uint8_t sr;
	status = ReadStatus(sr);
	if (status != StatusOk)
		return status;

	const unsigned int base = (density <= 5) ? 256 : ((density <= 7) ? 512 : 1024);
	pageSize = (sr & 0x01) ? base : (base + base / 32);	// bit 0: 1 = power of 2 page size
	pageCount = static_cast<unsigned int>((1UL << (15 + density)) / base);
	return StatusOk;
}

unsigned int At45::OffsetBits(void) const
{
	unsigned int bits = 0;
	while ((1u << bits) < pageSize)
		bits++;
	return bits;
}

bool At45::CheckRange(uint32_t addr, unsigned long count) const
{
	if (pageSize == 0)
		return false;
	if ((addr % pageSize) != 0 || (count % pageSize) != 0)
		return false;
	return (addr + static_cast<unsigned long>(count)) <= GetSize();
}

int At45::WaitReady(unsigned int timeoutMs, Progress *progress)
{
	const DWORD start = GetTickCount();
	for (;;)
	{
		uint8_t sr;
		int status = ReadStatus(sr);
		if (status != StatusOk)
			return status;
		if (sr & 0x80)	// RDY/BUSY: 1 = ready
			return StatusOk;
		if (Cancelled(progress))
			return StatusCancelled;
		if ((GetTickCount() - start) > timeoutMs)
			return StatusTimeout;
		Sleep(1);
	}
}

int At45::DisableProtection(void)
{
	uint8_t buf[4] = {0x3D, 0x2A, 0x7F, 0x9A};
	int status = MapError(ch341a.SpiTransfer(buf, sizeof(buf)));
	if (status != StatusOk)
		return status;
	return WaitReady(1000, NULL);
}

int At45::Read(uint32_t addr, uint8_t *data, unsigned long count, Progress *progress)
{
	if (!CheckRange(addr, count))
		return StatusInvalidArgs;

	const unsigned int shift = OffsetBits();
	unsigned int chunkPages = 2048 / pageSize;
	if (chunkPages == 0)
		chunkPages = 1;
	const unsigned long chunkBytes = static_cast<unsigned long>(chunkPages) * pageSize;

	std::vector<uint8_t> buf;
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		if (Cancelled(progress))
			return StatusCancelled;

		unsigned long n = count - done;
		if (n > chunkBytes)
			n = chunkBytes;

		const uint32_t page = (addr + done) / pageSize;
		const uint32_t a = page << shift;
		buf.assign(8 + n, 0xFF);
		buf[0] = 0xE8;	// continuous array read (legacy), 4 dummy bytes follow the address
		buf[1] = static_cast<uint8_t>(a >> 16);
		buf[2] = static_cast<uint8_t>(a >> 8);
		buf[3] = static_cast<uint8_t>(a);
		buf[4] = buf[5] = buf[6] = buf[7] = 0;
		int status = MapError(ch341a.SpiTransfer(&buf[0], static_cast<unsigned int>(8 + n)));
		if (status != StatusOk)
			return status;
		memcpy(data + done, &buf[8], n);
		done += n;
		Report(progress, done, count);
	}
	return StatusOk;
}

int At45::Write(uint32_t addr, const uint8_t *data, unsigned long count, Progress *progress)
{
	if (!CheckRange(addr, count))
		return StatusInvalidArgs;

	const unsigned int shift = OffsetBits();
	std::vector<uint8_t> buf(4 + pageSize);
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		if (Cancelled(progress))
			return StatusCancelled;

		const uint32_t page = (addr + done) / pageSize;
		const uint32_t a = page << shift;
		buf[0] = 0x82;
		buf[1] = static_cast<uint8_t>(a >> 16);
		buf[2] = static_cast<uint8_t>(a >> 8);
		buf[3] = static_cast<uint8_t>(a);
		memcpy(&buf[4], data + done, pageSize);
		int status = MapError(ch341a.SpiTransfer(&buf[0], static_cast<unsigned int>(buf.size())));
		if (status != StatusOk)
			return status;
		status = WaitReady(1000, progress);
		if (status != StatusOk)
			return status;

		done += pageSize;
		Report(progress, done, count);
	}
	return StatusOk;
}

int At45::EraseChip(Progress *progress)
{
	uint8_t buf[4] = {0xC7, 0x94, 0x80, 0x9A};
	int status = MapError(ch341a.SpiTransfer(buf, sizeof(buf)));
	if (status != StatusOk)
		return status;
	return WaitReady(5 * 60 * 1000, progress);
}

int At45::Verify(uint32_t addr, const uint8_t *data, unsigned long count, uint32_t &mismatchAddr, Progress *progress)
{
	if (!CheckRange(addr, count))
		return StatusInvalidArgs;

	std::vector<uint8_t> chunk;
	const unsigned long chunkBytes = static_cast<unsigned long>(pageSize) * ((2048 / pageSize) ? (2048 / pageSize) : 1);
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		unsigned long n = count - done;
		if (n > chunkBytes)
			n = chunkBytes;
		chunk.resize(n);
		int status = Read(addr + done, &chunk[0], n, NULL);
		if (status != StatusOk)
			return status;
		for (unsigned long i=0; i<n; i++)
		{
			if (chunk[i] != data[done + i])
			{
				mismatchAddr = addr + done + i;
				return StatusVerifyFailed;
			}
		}
		done += n;
		Report(progress, done, count);
		if (Cancelled(progress))
			return StatusCancelled;
	}
	return StatusOk;
}

//---------------------------------------------------------------------------
// SPI EEPROM
//---------------------------------------------------------------------------

static const SpiEepromType spiEepromTypes[] = {
	{ "25x010 (128 B)",   128UL,    16,  1 },
	{ "25x020 (256 B)",   256UL,    16,  1 },
	{ "25x040 (512 B)",   512UL,    16,  1 },	// A8 in opcode
	{ "25x080 (1 KB)",    1024UL,   32,  2 },
	{ "25x160 (2 KB)",    2048UL,   32,  2 },
	{ "25x320 (4 KB)",    4096UL,   32,  2 },
	{ "25x640 (8 KB)",    8192UL,   32,  2 },
	{ "25x128 (16 KB)",   16384UL,  64,  2 },
	{ "25x256 (32 KB)",   32768UL,  64,  2 },
	{ "25x512 (64 KB)",   65536UL,  128, 2 },
	{ "25x1024 (128 KB)", 131072UL, 256, 3 }
};

const SpiEepromType* GetSpiEepromTypes(unsigned int &count)
{
	count = sizeof(spiEepromTypes) / sizeof(spiEepromTypes[0]);
	return spiEepromTypes;
}

//---------------------------------------------------------------------------
// Microwire EEPROM
//---------------------------------------------------------------------------

static const MicrowireType microwireTypes[] = {
	{ "93C46 (128 B)",  128UL,  6,  7 },
	{ "93C56 (256 B)",  256UL,  8,  9 },
	{ "93C66 (512 B)",  512UL,  8,  9 },
	{ "93C76 (1 KB)",   1024UL, 10, 11 },
	{ "93C86 (2 KB)",   2048UL, 10, 11 }
};

const MicrowireType* GetMicrowireTypes(unsigned int &count)
{
	count = sizeof(microwireTypes) / sizeof(microwireTypes[0]);
	return microwireTypes;
}

namespace
{
	// CH341BitStreamSPI: one byte per clock; bit0 = CS (D0), bit5 = DOUT (D5), bit7 = DIN (D7, returned)
	const uint8_t BIT_CS = 0x01;
	const uint8_t BIT_DOUT = 0x20;
	const uint8_t BIT_DIN = 0x80;
	const uint32_t PINS_DIRECTION = 0x29;	// D0 (CS), D3 (SK), D5 (DI) are outputs
	const unsigned int MW_READ_CHUNK_BYTES = 16;
	const unsigned int MW_BIT_STREAM_MAX = 256;	// recommended maximum for CH341BitStreamSPI

	void AppendBits(std::vector<uint8_t> &bits, uint32_t value, unsigned int count)
	{
		for (unsigned int i=count; i>0; i--)
			bits.push_back(static_cast<uint8_t>(BIT_CS | (((value >> (i - 1)) & 1) ? BIT_DOUT : 0)));
	}

	/** Releases D5-D0 (all inputs) when leaving an operation */
	class PinsGuard
	{
	public:
		PinsGuard(void) {}
		~PinsGuard(void) { ch341a.SetD5D0(0, 0); }
	};
}

void Microwire::Configure(const MicrowireType *mwType, bool wordOrganisation16)
{
	type = mwType;
	x16 = wordOrganisation16;
}

unsigned long Microwire::GetSize(void) const
{
	return type ? type->size : 0;
}

unsigned int Microwire::AddrBits(void) const
{
	return x16 ? type->addrBitsX16 : type->addrBitsX8;
}

bool Microwire::CheckRange(uint32_t addr, unsigned long count) const
{
	if (!type || (addr + static_cast<unsigned long>(count)) > type->size)
		return false;
	if (x16 && (((addr | count) & 1) != 0))
		return false;
	return true;
}

int Microwire::Transfer(std::vector<uint8_t> &bits)
{
	// CS is set before data stream, since all the bytes in the stream have CS high
	int status = MapError(ch341a.SetD5D0(PINS_DIRECTION, 0));
	if (status != StatusOk)
		return status;
	status = MapError(ch341a.SetD5D0(PINS_DIRECTION, BIT_CS));
	if (status != StatusOk)
		return status;
	status = MapError(ch341a.SpiTransferBitStream(&bits[0], static_cast<unsigned int>(bits.size())));
	if (status != StatusOk)
		return status;
	return MapError(ch341a.SetD5D0(PINS_DIRECTION, 0));
}

int Microwire::WaitReady(Progress *progress)
{
	// DO shows busy (low) / ready (high) when CS goes high again after the write command
	int status = MapError(ch341a.SetD5D0(PINS_DIRECTION, BIT_CS));
	if (status != StatusOk)
		return status;
	const DWORD start = GetTickCount();
	for (;;)
	{
		uint32_t pins = 0;
		status = MapError(ch341a.GetGpioInputs(pins));
		if (status != StatusOk)
			break;
		if (pins & BIT_DIN)
			break;
		if (Cancelled(progress))
		{
			status = StatusCancelled;
			break;
		}
		if ((GetTickCount() - start) > 200)
		{
			status = StatusTimeout;
			break;
		}
		Sleep(1);
	}
	ch341a.SetD5D0(PINS_DIRECTION, 0);
	return status;
}

/** Commands without data: EWEN (opcode 0, field 3), EWDS (0, 0), ERAL (0, 2); field is placed in two address MSBs */
int Microwire::WriteCommand(unsigned int opcode, unsigned int addrField, bool waitReady, Progress *progress)
{
	std::vector<uint8_t> bits;
	AppendBits(bits, 1, 1);
	AppendBits(bits, opcode, 2);
	AppendBits(bits, addrField, 2);
	AppendBits(bits, 0, AddrBits() - 2);
	int status = Transfer(bits);
	if (status == StatusOk && waitReady)
		status = WaitReady(progress);
	return status;
}

int Microwire::Read(uint32_t addr, uint8_t *data, unsigned long count, Progress *progress)
{
	if (!CheckRange(addr, count))
		return StatusInvalidArgs;
	if (!ch341a.IsOpened())
		return StatusNotOpened;

	PinsGuard guard;
	std::vector<uint8_t> bits;
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		if (Cancelled(progress))
			return StatusCancelled;

		const uint32_t cur = addr + done;
		unsigned int n = MW_READ_CHUNK_BYTES - (cur % MW_READ_CHUNK_BYTES);
		if (n > count - done)
			n = static_cast<unsigned int>(count - done);

		// start bit + READ opcode (10) + address, then clock out data; the dummy zero bit
		// of the chip appears during the last address bit
		const unsigned int cmdBits = 3 + AddrBits();
		bits.clear();
		AppendBits(bits, 1, 1);
		AppendBits(bits, 2, 2);
		AppendBits(bits, x16 ? (cur >> 1) : cur, AddrBits());
		AppendBits(bits, 0, n * 8);
		if (bits.size() > MW_BIT_STREAM_MAX + 16)
			return StatusInvalidArgs;

		int status = Transfer(bits);
		if (status != StatusOk)
			return status;

		for (unsigned int i=0; i<n; i++)
		{
			uint8_t value = 0;
			for (unsigned int b=0; b<8; b++)
			{
				value = static_cast<uint8_t>(value << 1);
				if (bits[cmdBits + i * 8 + b] & BIT_DIN)
					value |= 1;
			}
			data[done + i] = value;
		}
		done += n;
		Report(progress, done, count);
	}
	return StatusOk;
}

int Microwire::Write(uint32_t addr, const uint8_t *data, unsigned long count, Progress *progress)
{
	if (!CheckRange(addr, count))
		return StatusInvalidArgs;
	if (!ch341a.IsOpened())
		return StatusNotOpened;

	PinsGuard guard;
	int status = WriteCommand(0, 3, false, progress);	// EWEN
	if (status != StatusOk)
		return status;

	const unsigned int unitBytes = x16 ? 2 : 1;
	std::vector<uint8_t> bits;
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		if (Cancelled(progress))
		{
			status = StatusCancelled;
			break;
		}

		const uint32_t cur = addr + done;
		uint32_t value = data[done];
		if (x16)
			value = (value << 8) | data[done + 1];

		bits.clear();
		AppendBits(bits, 1, 1);
		AppendBits(bits, 1, 2);	// WRITE
		AppendBits(bits, x16 ? (cur >> 1) : cur, AddrBits());
		AppendBits(bits, value, unitBytes * 8);
		status = Transfer(bits);
		if (status == StatusOk)
			status = WaitReady(progress);
		if (status != StatusOk)
			break;

		done += unitBytes;
		Report(progress, done, count);
	}

	WriteCommand(0, 0, false, NULL);	// EWDS, regardless of result
	return status;
}

int Microwire::Erase(Progress *progress)
{
	if (!type)
		return StatusInvalidArgs;
	if (!ch341a.IsOpened())
		return StatusNotOpened;

	PinsGuard guard;
	int status = WriteCommand(0, 3, false, progress);	// EWEN
	if (status == StatusOk)
		status = WriteCommand(0, 2, true, progress);	// ERAL
	WriteCommand(0, 0, false, NULL);	// EWDS
	return status;
}

int Microwire::Verify(uint32_t addr, const uint8_t *data, unsigned long count, uint32_t &mismatchAddr, Progress *progress)
{
	if (!CheckRange(addr, count))
		return StatusInvalidArgs;

	std::vector<uint8_t> chunk;
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		unsigned int n = static_cast<unsigned int>(count - done);
		if (n > 256)
			n = 256;
		chunk.resize(n);
		int status = Read(addr + done, &chunk[0], n, NULL);
		if (status != StatusOk)
			return status;
		for (unsigned int i=0; i<n; i++)
		{
			if (chunk[i] != data[done + i])
			{
				mismatchAddr = addr + done + i;
				return StatusVerifyFailed;
			}
		}
		done += n;
		Report(progress, done, count);
		if (Cancelled(progress))
			return StatusCancelled;
	}
	return StatusOk;
}

//---------------------------------------------------------------------------
// I2C EEPROM
//---------------------------------------------------------------------------

static const I2cEepromType i2cEepromTypes[] = {
	{ "24C01",   128UL,    8,   1, 0 },
	{ "24C02",   256UL,    8,   1, 0 },
	{ "24C04",   512UL,    16,  1, 1 },
	{ "24C08",   1024UL,   16,  1, 2 },
	{ "24C16",   2048UL,   16,  1, 3 },
	{ "24C32",   4096UL,   32,  2, 0 },
	{ "24C64",   8192UL,   32,  2, 0 },
	{ "24C128",  16384UL,  64,  2, 0 },
	{ "24C256",  32768UL,  64,  2, 0 },
	{ "24C512",  65536UL,  128, 2, 0 },
	{ "24C1024", 131072UL, 256, 2, 1 }	// A16 in device address (e.g. 24LC1026, M24M01)
};

const I2cEepromType* GetI2cEepromTypes(unsigned int &count)
{
	count = sizeof(i2cEepromTypes) / sizeof(i2cEepromTypes[0]);
	return i2cEepromTypes;
}

void I2cEeprom::Configure(const I2cEepromType *eepromType, uint8_t baseAddr7bit, bool autoAddressSelection)
{
	type = eepromType;
	devAddr = static_cast<uint8_t>(baseAddr7bit & 0x7F);
	autoAddress = autoAddressSelection;
}

int I2cEeprom::Scan(I2cScanResult &result)
{
	result.ackMask = 0;
	result.blocks = 0;
	result.baseAddr = 0;
	result.valid = false;
	if (!ch341a.IsOpened())
		return StatusNotOpened;

	for (unsigned int i=0; i<8; i++)
	{
		if (ch341a.I2CCheckDev(static_cast<uint8_t>(0x50 + i)) == 0)
			result.ackMask |= (1u << i);
	}
	if (result.ackMask == 0)
		return StatusOk;

	unsigned int first = 0;
	while ((result.ackMask & (1u << first)) == 0)
		first++;
	unsigned int run = 0;
	while ((first + run) < 8 && (result.ackMask & (1u << (first + run))) != 0)
		run++;

	result.baseAddr = static_cast<uint8_t>(0x50 + first);
	result.blocks = run;
	const unsigned int runMask = ((1u << run) - 1) << first;
	result.valid = (result.ackMask == runMask) && (run == 1 || run == 2 || run == 4 || run == 8) && ((first % run) == 0);
	return StatusOk;
}

unsigned long I2cEeprom::GetSize(void) const
{
	return type ? type->size : 0;
}

uint8_t I2cEeprom::DeviceAddr7(uint32_t addr) const
{
	const unsigned int shift = type->addrBytes * 8;
	const unsigned int mask = (1u << type->addrBitsInDev) - 1;
	return static_cast<uint8_t>((devAddr & ~mask) | ((addr >> shift) & mask));
}

unsigned int I2cEeprom::BuildHeader(uint8_t *buf, uint32_t addr) const
{
	unsigned int n = 0;
	buf[n++] = static_cast<uint8_t>(DeviceAddr7(addr) << 1);
	if (type->addrBytes == 2)
		buf[n++] = static_cast<uint8_t>(addr >> 8);
	buf[n++] = static_cast<uint8_t>(addr);
	return n;
}

int I2cEeprom::Probe(void)
{
	if (!type)
		return StatusInvalidArgs;
	int status = Scan(scan);
	if (status != StatusOk)
		return status;
	if (scan.ackMask == 0)
		return StatusNoDevice;

	const unsigned int expectedBlocks = 1u << type->addrBitsInDev;
	if (!scan.valid || scan.blocks != expectedBlocks)
		return StatusWrongDevice;

	if (autoAddress)
		devAddr = scan.baseAddr;
	else if (scan.baseAddr != static_cast<uint8_t>(devAddr & ~(expectedBlocks - 1)))
		return StatusNoDevice;
	return StatusOk;
}

int I2cEeprom::WaitWriteDone(uint32_t addr, Progress *progress)
{
	const DWORD start = GetTickCount();
	const uint8_t dev = DeviceAddr7(addr);
	// internal write cycle takes 5..10 ms, device does not ACK meanwhile
	for (;;)
	{
		if (ch341a.I2CCheckDev(dev) == 0)
			return StatusOk;
		if (Cancelled(progress))
			return StatusCancelled;
		if ((GetTickCount() - start) > 100)
			return StatusTimeout;
		Sleep(1);
	}
}

int I2cEeprom::Read(uint32_t addr, uint8_t *data, unsigned long count, Progress *progress)
{
	if (!type || (addr + static_cast<unsigned long>(count)) > type->size)
		return StatusInvalidArgs;

	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		if (Cancelled(progress))
			return StatusCancelled;

		const uint32_t cur = addr + done;
		unsigned int n = I2C_READ_CHUNK - (cur % I2C_READ_CHUNK);	// stay inside one device address block
		if (n > count - done)
			n = static_cast<unsigned int>(count - done);

		uint8_t hdr[3];
		unsigned int hdrLen = BuildHeader(hdr, cur);
		int status = MapError(ch341a.I2CWriteRead(hdr, hdrLen, data + done, n));
		if (status != StatusOk)
			return status;
		done += n;
		Report(progress, done, count);
	}
	return StatusOk;
}

int I2cEeprom::Write(uint32_t addr, const uint8_t *data, unsigned long count, bool skipBlank, Progress *progress)
{
	if (!type || (addr + static_cast<unsigned long>(count)) > type->size)
		return StatusInvalidArgs;

	std::vector<uint8_t> buf;
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		if (Cancelled(progress))
			return StatusCancelled;

		const uint32_t cur = addr + done;
		unsigned int n = type->pageSize - (cur % type->pageSize);
		if (n > count - done)
			n = static_cast<unsigned int>(count - done);

		if (!(skipBlank && IsBlank(data + done, n)))
		{
			buf.resize(3 + n);
			unsigned int hdrLen = BuildHeader(&buf[0], cur);
			memcpy(&buf[hdrLen], data + done, n);
			int status = MapError(ch341a.I2CWriteRead(&buf[0], hdrLen + n, NULL, 0));
			if (status != StatusOk)
				return status;
			status = WaitWriteDone(cur, progress);
			if (status != StatusOk)
				return status;
		}
		done += n;
		Report(progress, done, count);
	}
	return StatusOk;
}

int I2cEeprom::Erase(Progress *progress)
{
	if (!type)
		return StatusInvalidArgs;
	std::vector<uint8_t> blank(type->size, 0xFF);
	return Write(0, &blank[0], blank.size(), false, progress);
}

int I2cEeprom::Verify(uint32_t addr, const uint8_t *data, unsigned long count, uint32_t &mismatchAddr, Progress *progress)
{
	if (!type || (addr + static_cast<unsigned long>(count)) > type->size)
		return StatusInvalidArgs;

	std::vector<uint8_t> chunk;
	unsigned long done = 0;
	Report(progress, 0, count);
	while (done < count)
	{
		unsigned int n = static_cast<unsigned int>(count - done);
		if (n > I2C_READ_CHUNK * 4)
			n = I2C_READ_CHUNK * 4;
		chunk.resize(n);
		int status = Read(addr + done, &chunk[0], n, NULL);
		if (status != StatusOk)
			return status;
		for (unsigned int i=0; i<n; i++)
		{
			if (chunk[i] != data[done + i])
			{
				mismatchAddr = addr + done + i;
				return StatusVerifyFailed;
			}
		}
		done += n;
		Report(progress, done, count);
		if (Cancelled(progress))
			return StatusCancelled;
	}
	return StatusOk;
}

}	// namespace MemProg
