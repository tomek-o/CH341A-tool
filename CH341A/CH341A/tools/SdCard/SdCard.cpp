//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SdCard.h"
#include "CH341A.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace SdMmc
{

using namespace MemProg;

namespace
{
	enum
	{
		CMD0 = 0, CMD1 = 1, CMD8 = 8, CMD9 = 9, CMD10 = 10, CMD13 = 13, CMD16 = 16,
		CMD17 = 17, CMD24 = 24, CMD55 = 55, CMD58 = 58, ACMD41 = 41
	};
	const unsigned int R1_WAIT_BYTES = 16;		// R1 comes 1..8 bytes after the command
	const unsigned int TOKEN_WAIT_BYTES = 256;	// data token follows R1 after a delay
	const unsigned int POLL_BYTES = 512;
	const uint8_t DATA_TOKEN = 0xFE;

	// CH341BitStreamSPI: one byte per clock; bit0 = CS (D0), bit5 = DOUT (D5)
	const uint8_t BIT_CS = 0x01;
	const uint8_t BIT_DOUT = 0x20;
	const uint32_t PINS_DIRECTION = 0x29;	// D0 (CS), D3 (SCK), D5 (MOSI) are outputs

	int MapError(int result)
	{
		if (result == 0)
			return StatusOk;
		return (result == -1) ? StatusNotOpened : StatusCommError;
	}

	std::string Fmt(const char *format, ...);

	std::string Hex(const uint8_t *data, unsigned int count)
	{
		std::string s;
		char b[4];
		for (unsigned int i=0; i<count; i++)
		{
			sprintf(b, "%02X", data[i]);
			if (i)
				s += ' ';
			s += b;
		}
		return s;
	}

	uint8_t Crc7(const uint8_t *data, unsigned int count)
	{
		uint8_t crc = 0;
		for (unsigned int i=0; i<count; i++)
		{
			uint8_t d = data[i];
			for (int bit=0; bit<8; bit++)
			{
				crc <<= 1;
				if ((d ^ crc) & 0x80)
					crc ^= 0x09;
				d <<= 1;
			}
		}
		return static_cast<uint8_t>(crc & 0x7F);
	}

	bool Cancelled(Progress *progress)
	{
		return progress && progress->IsCancelled();
	}
}

SdCard sdCard;

const char* GetConnectionText(void)
{
	return
		"SD card in SPI mode (3.3 V only!): card CS (microSD CD/DAT3) to CH341A CS0, DI (CMD) to MOSI (D5), "
		"CLK to SCK (D3), DO (DAT0) to MISO (D7), VDD to 3.3 V, GND to GND. 10k pull-up on CS recommended, "
		"DAT1/DAT2 stay open.";
}

void SdCard::Log(const std::string &line)
{
	if (trace)
		trace->Trace(line);
}

int SdCard::Command(uint8_t cmd, uint32_t arg, SdResponse type, uint8_t &r1, uint8_t *extra)
{
	const unsigned int extraCount = (type == SdR3R7) ? 4 : (type == SdR2) ? 1 : 0;
	std::vector<uint8_t> buf(7 + R1_WAIT_BYTES + extraCount, 0xFF);
	buf[1] = static_cast<uint8_t>(0x40 | (cmd & 0x3F));
	buf[2] = static_cast<uint8_t>(arg >> 24);
	buf[3] = static_cast<uint8_t>(arg >> 16);
	buf[4] = static_cast<uint8_t>(arg >> 8);
	buf[5] = static_cast<uint8_t>(arg);
	buf[6] = static_cast<uint8_t>((cmd == CMD0) ? 0x95 : ((cmd == CMD8 && arg == 0x1AA) ? 0x87 : 0x01));

	r1 = 0xFF;
	int status = MapError(ch341a.SpiTransfer(&buf[0], static_cast<unsigned int>(buf.size())));
	if (status != StatusOk)
		return status;

	unsigned int pos = 0;
	for (unsigned int i=7; i<7+R1_WAIT_BYTES; i++)
	{
		if ((buf[i] & 0x80) == 0)
		{
			r1 = buf[i];
			pos = i;
			break;
		}
	}
	std::string line = Fmt("CMD%u arg=%08lX crc=%02X -> ", static_cast<unsigned>(cmd), static_cast<unsigned long>(arg), buf[6]);
	if (pos == 0)
	{
		Log(line + "no response (card absent, not powered, or wiring)");
		return StatusNoDevice;
	}
	line += Fmt("R1=%02X", r1);
	if (extraCount)
	{
		memcpy(extra, &buf[pos + 1], extraCount);
		line += " " + Hex(extra, extraCount);
	}
	std::string d = DescribeR1(r1);
	if (!d.empty())
		line += "  (" + d + ")";
	Log(line);
	return StatusOk;
}

int SdCard::AppCommand(uint8_t cmd, uint32_t arg, SdResponse type, uint8_t &r1, uint8_t *extra)
{
	int status = Command(CMD55, 0, SdR1, r1, NULL);
	if (status != StatusOk)
		return status;
	if (r1 & 0x7E)
		return StatusCommError;
	return Command(cmd, arg, type, r1, extra);
}

int SdCard::Init(Progress *progress)
{
	initialized = false;
	highCapacity = false;
	version2 = false;
	ocr = 0;

	// 80 clocks with CS and MOSI high: the card needs 74+ before CMD0
	std::vector<uint8_t> bits(80, static_cast<uint8_t>(BIT_CS | BIT_DOUT));
	int status = MapError(ch341a.SetD5D0(PINS_DIRECTION, BIT_CS));
	if (status != StatusOk)
		return status;
	status = MapError(ch341a.SpiTransferBitStream(&bits[0], static_cast<unsigned int>(bits.size())));
	if (status != StatusOk)
		return status;
	ch341a.SetD5D0(BIT_CS, BIT_CS);	// keep CS high, release the rest
	Log("80 clocks sent with CS high");

	uint8_t r1 = 0xFF;
	uint8_t ext[4];
	bool idle = false;
	for (int i=0; i<10 && !idle; i++)
	{
		status = Command(CMD0, 0, SdR1, r1, NULL);
		if (status != StatusOk && status != StatusNoDevice)
			return status;
		idle = (status == StatusOk && r1 == 0x01);
		if (!idle && Cancelled(progress))
			return StatusCancelled;
	}
	if (!idle)
		return StatusNoDevice;

	status = Command(CMD8, 0x1AA, SdR3R7, r1, ext);
	if (status != StatusOk)
		return status;
	if (r1 & 0x04)
		Log("CMD8 rejected: SD version 1 card (or MMC)");
	else if (ext[2] == 0x01 && ext[3] == 0xAA)
	{
		version2 = true;
		Log("CMD8 echo OK: SD version 2 card");
	}
	else
	{
		Log("CMD8 echo mismatch, card not usable");
		return StatusCommError;
	}

	const DWORD start = GetTickCount();
	bool ready = false;
	bool mmc = false;
	for (;;)
	{
		status = AppCommand(ACMD41, version2 ? 0x40000000UL : 0, SdR1, r1, NULL);
		if (status == StatusOk && (r1 & 0x04))
		{
			mmc = true;	// ACMD41 unknown: MMC card
			break;
		}
		if (status == StatusOk && r1 == 0x00)
		{
			ready = true;
			break;
		}
		if (Cancelled(progress))
			return StatusCancelled;
		if ((GetTickCount() - start) > 2000)
			return StatusTimeout;
		Sleep(20);
	}
	if (mmc)
	{
		Log("MMC card: using CMD1");
		for (;;)
		{
			status = Command(CMD1, 0x40000000UL, SdR1, r1, NULL);
			if (status == StatusOk && r1 == 0x00)
			{
				ready = true;
				break;
			}
			if (Cancelled(progress))
				return StatusCancelled;
			if ((GetTickCount() - start) > 2000)
				return StatusTimeout;
			Sleep(20);
		}
	}
	if (!ready)
		return StatusTimeout;

	status = ReadOcr(ocr);
	if (status == StatusOk && version2 && (ocr & 0x40000000UL))
		highCapacity = true;
	Log(highCapacity ? "SDHC/SDXC: block addressing" : "SDSC: byte addressing");

	if (!highCapacity)
		Command(CMD16, SECTOR_SIZE, SdR1, r1, NULL);

	initialized = true;
	return StatusOk;
}

int SdCard::ReadOcr(uint32_t &value)
{
	uint8_t r1, ext[4];
	int status = Command(CMD58, 0, SdR3R7, r1, ext);
	if (status != StatusOk)
		return status;
	if (r1 & 0x7E)
		return StatusCommError;
	value = (static_cast<uint32_t>(ext[0]) << 24) | (static_cast<uint32_t>(ext[1]) << 16) |
		(static_cast<uint32_t>(ext[2]) << 8) | ext[3];
	ocr = value;
	Log("OCR: " + DescribeOcr(value));
	return StatusOk;
}

int SdCard::ReadStatus(uint8_t &r1, uint8_t &r2)
{
	uint8_t ext[1];
	int status = Command(CMD13, 0, SdR2, r1, ext);
	r2 = ext[0];
	return status;
}

int SdCard::CommandReadBlock(uint8_t cmd, uint32_t arg, uint8_t *data, unsigned int length)
{
	const unsigned int total = 7 + R1_WAIT_BYTES + TOKEN_WAIT_BYTES + length + 2;
	std::vector<uint8_t> buf(total, 0xFF);
	buf[1] = static_cast<uint8_t>(0x40 | (cmd & 0x3F));
	buf[2] = static_cast<uint8_t>(arg >> 24);
	buf[3] = static_cast<uint8_t>(arg >> 16);
	buf[4] = static_cast<uint8_t>(arg >> 8);
	buf[5] = static_cast<uint8_t>(arg);
	buf[6] = 0x01;

	int status = MapError(ch341a.SpiTransfer(&buf[0], total));
	if (status != StatusOk)
		return status;

	unsigned int pos = 0;
	uint8_t r1 = 0xFF;
	for (unsigned int i=7; i<7+R1_WAIT_BYTES; i++)
	{
		if ((buf[i] & 0x80) == 0)
		{
			r1 = buf[i];
			pos = i;
			break;
		}
	}
	std::string line = Fmt("CMD%u arg=%08lX -> ", static_cast<unsigned>(cmd), static_cast<unsigned long>(arg));
	if (pos == 0)
	{
		Log(line + "no response");
		return StatusNoDevice;
	}
	line += Fmt("R1=%02X", r1);
	if (r1 != 0)
	{
		Log(line + "  (" + DescribeR1(r1) + ")");
		return StatusCommError;
	}
	unsigned int token = 0;
	for (unsigned int i=pos+1; i+length+2<=total; i++)
	{
		if (buf[i] != 0xFF)
		{
			token = i;
			break;
		}
	}
	if (token == 0)
	{
		Log(line + ", no data token (timeout)");
		return StatusTimeout;
	}
	if (buf[token] != DATA_TOKEN)
	{
		Log(line + Fmt(", error token %02X", buf[token]));
		return StatusCommError;
	}
	if (token + 1 + length > total)
	{
		Log(line + ", token too late");
		return StatusTimeout;
	}
	memcpy(data, &buf[token + 1], length);
	Log(line + Fmt(", data after %u byte(s)", token - pos - 1));
	return StatusOk;
}

int SdCard::ReadCid(SdCid &cid)
{
	int status = CommandReadBlock(CMD10, 0, cid.raw, 16);
	if (status != StatusOk)
		return status;
	cid.manufacturer = cid.raw[0];
	cid.oem[0] = static_cast<char>(cid.raw[1]);
	cid.oem[1] = static_cast<char>(cid.raw[2]);
	cid.oem[2] = 0;
	memcpy(cid.product, &cid.raw[3], 5);
	cid.product[5] = 0;
	cid.revMajor = cid.raw[8] >> 4;
	cid.revMinor = cid.raw[8] & 0x0F;
	cid.serial = (static_cast<uint32_t>(cid.raw[9]) << 24) | (static_cast<uint32_t>(cid.raw[10]) << 16) |
		(static_cast<uint32_t>(cid.raw[11]) << 8) | cid.raw[12];
	cid.year = 2000 + (((cid.raw[13] & 0x0F) << 4) | (cid.raw[14] >> 4));
	cid.month = cid.raw[14] & 0x0F;
	cid.crcOk = (Crc7(cid.raw, 15) == (cid.raw[15] >> 1));
	return StatusOk;
}

int SdCard::ReadCsd(SdCsd &csd)
{
	int status = CommandReadBlock(CMD9, 0, csd.raw, 16);
	if (status != StatusOk)
		return status;
	const uint8_t *c = csd.raw;
	csd.version = 0;
	csd.capacityBytes = 0;
	csd.readBlockLen = 1u << (c[5] & 0x0F);
	switch (c[0] >> 6)
	{
	case 0:
		{
			csd.version = 1;
			const uint32_t cSize = ((c[6] & 3u) << 10) | (static_cast<uint32_t>(c[7]) << 2) | (c[8] >> 6);
			const unsigned int mult = ((c[9] & 3u) << 1) | (c[10] >> 7);
			csd.capacityBytes = (static_cast<uint64_t>(cSize) + 1) << (mult + 2);
			csd.capacityBytes *= csd.readBlockLen;
		}
		break;
	case 1:
		{
			csd.version = 2;
			const uint32_t cSize = ((c[7] & 0x3Fu) << 16) | (static_cast<uint32_t>(c[8]) << 8) | c[9];
			csd.capacityBytes = (static_cast<uint64_t>(cSize) + 1) * 512 * 1024;
			csd.readBlockLen = 512;
		}
		break;
	default:
		break;
	}
	return StatusOk;
}

int SdCard::ReadSectors(uint32_t sector, uint32_t count, uint8_t *data, Progress *progress)
{
	if (!initialized)
		return StatusNoDevice;
	if (progress)
		progress->Report(0, count);
	for (uint32_t i=0; i<count; i++)
	{
		if (Cancelled(progress))
			return StatusCancelled;
		int status = CommandReadBlock(CMD17, Address(sector + i), data + static_cast<size_t>(i) * SECTOR_SIZE, SECTOR_SIZE);
		if (status != StatusOk)
			return status;
		if (progress)
			progress->Report(i + 1, count);
	}
	return StatusOk;
}

int SdCard::WriteSectors(uint32_t sector, uint32_t count, const uint8_t *data, Progress *progress)
{
	if (!initialized)
		return StatusNoDevice;
	if (progress)
		progress->Report(0, count);
	for (uint32_t i=0; i<count; i++)
	{
		if (Cancelled(progress))
			return StatusCancelled;
		uint8_t r1;
		int status = Command(CMD24, Address(sector + i), SdR1, r1, NULL);
		if (status != StatusOk)
			return status;
		if (r1 != 0)
			return StatusCommError;

		// token + data + dummy CRC + room for the data response, then busy polling in the same CS window
		std::vector<uint8_t> buf(1 + 1 + SECTOR_SIZE + 2 + POLL_BYTES, 0xFF);
		buf[1] = DATA_TOKEN;
		memcpy(&buf[2], data + static_cast<size_t>(i) * SECTOR_SIZE, SECTOR_SIZE);
		status = MapError(ch341a.SpiTransfer(&buf[0], static_cast<unsigned int>(buf.size())));
		if (status != StatusOk)
			return status;
		const uint8_t response = buf[2 + SECTOR_SIZE + 2];
		Log(Fmt("write sector %lu: data response %02X", static_cast<unsigned long>(sector + i), response));
		if ((response & 0x1F) != 0x05)
			return StatusCommError;

		// busy: card holds DO low; the last byte of a poll call is FF once it is done
		bool busy = (buf[buf.size() - 1] != 0xFF);
		const DWORD start = GetTickCount();
		while (busy)
		{
			if (Cancelled(progress))
				return StatusCancelled;
			if ((GetTickCount() - start) > 3000)
				return StatusTimeout;
			std::vector<uint8_t> poll(POLL_BYTES, 0xFF);
			status = MapError(ch341a.SpiTransfer(&poll[0], static_cast<unsigned int>(poll.size())));
			if (status != StatusOk)
				return status;
			busy = (poll[poll.size() - 1] != 0xFF);
		}
		if (progress)
			progress->Report(i + 1, count);
	}
	return StatusOk;
}

//---------------------------------------------------------------------------

namespace
{
	std::string Fmt(const char *format, ...)
	{
		char buf[256];
		va_list args;
		va_start(args, format);
		vsnprintf(buf, sizeof(buf), format, args);
		va_end(args);
		buf[sizeof(buf) - 1] = 0;
		return std::string(buf);
	}
}

std::string SdCard::DescribeR1(uint8_t r1)
{
	std::string s;
	static const char *const names[] = { "idle", "erase reset", "illegal command", "CRC error",
		"erase sequence error", "address error", "parameter error" };
	for (int i=0; i<7; i++)
	{
		if (r1 & (1 << i))
		{
			if (!s.empty())
				s += ", ";
			s += names[i];
		}
	}
	return s;
}

std::string SdCard::DescribeOcr(uint32_t ocr)
{
	std::string s = Fmt("%08lX", static_cast<unsigned long>(ocr));
	s += (ocr & 0x80000000UL) ? ", powered up" : ", busy";
	s += (ocr & 0x40000000UL) ? ", CCS=1 (SDHC/SDXC)" : ", CCS=0 (SDSC)";
	s += ", 3.3V range: ";
	s += (ocr & 0x00FF8000UL) ? "yes" : "no";
	return s;
}

std::string SdCard::DescribeCid(const SdCid &cid)
{
	std::string s = "CID " + Hex(cid.raw, 16) + "\r\n";
	s += Fmt("  manufacturer ID 0x%02X, OEM '%s', product '%s', rev %u.%u\r\n", cid.manufacturer, cid.oem, cid.product, cid.revMajor, cid.revMinor);
	s += Fmt("  serial %08lX, date %04u-%02u, CRC7 %s", static_cast<unsigned long>(cid.serial), cid.year, cid.month, cid.crcOk ? "OK" : "BAD");
	return s;
}

std::string SdCard::DescribeCsd(const SdCsd &csd)
{
	std::string s = "CSD " + Hex(csd.raw, 16) + "\r\n";
	if (csd.version == 0)
		return s + "  unknown CSD structure";
	s += Fmt("  CSD v%u, read block %u bytes, capacity %.1f MB (%lu sectors)", csd.version, csd.readBlockLen,
		static_cast<double>(csd.capacityBytes) / (1024.0 * 1024.0), static_cast<unsigned long>(csd.capacityBytes / 512));
	return s;
}

}	// namespace SdMmc
