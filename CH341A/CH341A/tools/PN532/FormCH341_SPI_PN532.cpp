//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341_SPI_PN532.h"
#include "PN532.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/ScopedBool.h"
#include "common/bin2str.h"
#include "Log.h"
#include <assert.h>
#include <vector>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341_SPI_PN532 *frmCH341_SPI_PN532;
//---------------------------------------------------------------------------

namespace
{
// NFC Forum URI Record Type Definition abbreviation table (payload byte 0).
const char* NdefUriPrefix(unsigned char code)
{
	static const char* const prefixes[] = {
		"", "http://www.", "https://www.", "http://", "https://", "tel:",
		"mailto:", "ftp://anonymous:anonymous@", "ftp://ftp.", "ftps://",
		"sftp://", "smb://", "nfs://", "ftp://", "dav://", "news:",
		"telnet://", "imap:", "rtsp://", "urn:", "pop:", "sip:", "sips:",
		"tftp:", "btspp://", "btl2cap://", "btgoep://", "tcpobex://",
		"irdaobex://", "file://", "urn:epc:id:", "urn:epc:tag:",
		"urn:epc:pat:", "urn:epc:raw:", "urn:epc:", "urn:nfc:"
	};
	if (code < (sizeof(prefixes) / sizeof(prefixes[0])))
		return prefixes[code];
	return "";
}

// Decode one NDEF message (record by record) into human-readable text.
void ParseNdefMessage(const unsigned char *msg, unsigned int len, AnsiString &out)
{
	unsigned int i = 0;
	int recNo = 0;
	while (i < len)
	{
		unsigned char hdr = msg[i++];
		bool sr = (hdr & 0x10) != 0;	// short record
		bool il = (hdr & 0x08) != 0;	// ID length present
		unsigned char tnf = static_cast<unsigned char>(hdr & 0x07);

		if (i >= len) break;
		unsigned char typeLen = msg[i++];

		unsigned int payloadLen;
		if (sr)
		{
			if (i >= len) break;
			payloadLen = msg[i++];
		}
		else
		{
			if (i + 4 > len) break;
			payloadLen = ((unsigned int)msg[i] << 24) | ((unsigned int)msg[i+1] << 16) |
						 ((unsigned int)msg[i+2] << 8) | msg[i+3];
			i += 4;
		}

		unsigned char idLen = 0;
		if (il)
		{
			if (i >= len) break;
			idLen = msg[i++];
		}

		if (i + typeLen > len) break;
		AnsiString type;
		for (unsigned char k = 0; k < typeLen; k++)
			type += (char)msg[i + k];
		i += typeLen;
		i += idLen;	// skip the ID field

		if (i + payloadLen > len)
			payloadLen = (len > i) ? (len - i) : 0;	// clamp to what we actually have
		const unsigned char *payload = msg + i;

		recNo++;
		out.cat_printf("Record %d: TNF %d, type '%s'\n", recNo, (int)tnf, type.c_str());
		if (tnf == 0x01 && type == "U" && payloadLen >= 1)
		{
			AnsiString uri = NdefUriPrefix(payload[0]);
			for (unsigned int k = 1; k < payloadLen; k++)
				uri += (char)payload[k];
			out.cat_printf("  URI: %s\n", uri.c_str());
		}
		else if (tnf == 0x01 && type == "T" && payloadLen >= 1)
		{
			unsigned char langLen = static_cast<unsigned char>(payload[0] & 0x3F);
			AnsiString lang;
			for (unsigned char k = 0; k < langLen && (unsigned int)(1 + k) < payloadLen; k++)
				lang += (char)payload[1 + k];
			AnsiString txt;
			for (unsigned int k = 1 + langLen; k < payloadLen; k++)
				txt += (char)payload[k];
			out.cat_printf("  Text [%s]: %s\n", lang.c_str(), txt.c_str());
		}
		else if (payloadLen > 0)
		{
			out.cat_printf("  Payload: %s\n",
				BufToSpaceSeparatedHexString(payload, payloadLen).c_str());
		}

		i += payloadLen;
		if (hdr & 0x40)	// ME (message end)
			break;
	}
}
}	// namespace


__fastcall TfrmCH341_SPI_PN532::TfrmCH341_SPI_PN532(TComponent* Owner)
	: TForm(Owner),
	pn532(PN532::INTERFACE_SPI),
	reading(false),
	busy(false)
{
	TabManager::Instance().Register(this, 1u << ToolGroupMisc);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_SPI_PN532::btnStartClick(TObject *Sender)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	lblStatus->Caption = "";

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	LOG("PN532 (SPI): begin\n");
	int status = pn532.begin();
	if (status != 0)
	{
		lblStatus->Caption = "Failed to init / wake up PN532";
		return;
	}

	uint32_t versiondata = pn532.getFirmwareVersion();
	if (! versiondata) {
		lblStatus->Caption = "Failed to get PN532 firmware version!";
		return;
	}

	// Got ok data, print it out!
	version.sprintf("Found PN5%02X , version %d.%d",
		(versiondata>>24) & 0xFF,
		(versiondata>>16) & 0xFF,
		(versiondata>>8) & 0xFF
	);
	lblStatus->Caption = version;

	// Limit passive activation retries so InListPassiveTarget completes by
	// itself (reporting 0 targets) when no card is present. 0xFF would mean
	// retry forever - the chip then stays busy until the command is aborted.
	pn532.setPassiveActivationRetries(0x10);

	reading = true;
	lblReadingState->Caption = "Reading...";
	btnStart->Enabled = false;
	btnStop->Enabled = true;
	tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_SPI_PN532::btnStopClick(TObject *Sender)
{
	reading = false;
	tmrAutoRead->Enabled = false;
	lblReadingState->Caption = "Stopped";
	btnStop->Enabled = false;
	btnStart->Enabled = true;
}
//---------------------------------------------------------------------------

void TfrmCH341_SPI_PN532::Read(void)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	bool success;
	uint8_t uid[PN532_UID_MAX_LEN] = { 0 };	// Buffer to store the returned UID
	uint8_t uidLength;				// Length of the UID (4, 7 or 10 bytes depending on ISO14443A card type)

	// Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
	// 'uid' will be populated with the UID, and uidLength will indicate
	// if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
	success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 1000);

	if (success)
	{
		uint16_t atqa = pn532.getLastAtqa();
		uint8_t sak = pn532.getLastSak();

		AnsiString text;
		text.cat_printf("Found card:\n");
		text.cat_printf("UID Value: %s\n", BufToSpaceSeparatedHexString(uid, uidLength).c_str());

		// UID length also tells whether a manufacturer byte is present: a 7- or
		// 10-byte UID carries it in byte 0, a 4-byte UID is usually a random/NUID.
		if (uidLength == 4)
		{
			if (uid[0] == 0x08)
				text.cat_printf("UID: 4 bytes (random - changes on every read)\n");
			else
				text.cat_printf("UID: 4 bytes (fixed NUID, no manufacturer byte)\n");
		}
		else
		{
			text.cat_printf("UID: %d bytes, manufacturer: %s (0x%02X)\n",
				uidLength, PN532::describeManufacturer(uid[0]), uid[0]);
		}

		text.cat_printf("ATQA: %02X %02X, SAK: %02X\n", (atqa >> 8) & 0xFF, atqa & 0xFF, sak);
		text.cat_printf("Probable type: %s", PN532::describeIso14443aCard(atqa, sak));

		const char *mem = PN532::describeMifareMemory(sak);
		if (*mem)
			text.cat_printf("\nMemory: %s", mem);

		// NTAG / Ultralight (SAK 0x00, ATQA 0x0044): GET_VERSION pins down the
		// exact model and capacity, which SAK alone cannot.
		if (sak == 0x00 && atqa == 0x0044)
		{
			uint8_t ver[8];
			if (pn532.ntag2xx_GetVersion(ver))
				text.cat_printf("\nModel: %s", PN532::describeNtagFromVersion(ver));
		}

		memo->Lines->Text = text;	///< \note if assigning to memo->Text \n newlines would not be properly interpreted
	}
	else
	{
		memo->Lines->Text = "No card found";
	}
}
//---------------------------------------------------------------------------

bool TfrmCH341_SPI_PN532::ReadNtagBytes(std::vector<unsigned char> &data,
	unsigned int need, unsigned int capBytes)
{
	while (data.size() < need && data.size() < capBytes)
	{
		uint8_t page = static_cast<uint8_t>(4 + data.size() / 4);
		uint8_t buf[4];
		if (!pn532.ntag2xx_ReadPage(page, buf))
			return false;
		for (int i = 0; i < 4; i++)
			data.push_back(buf[i]);
	}
	return data.size() >= need;
}
//---------------------------------------------------------------------------

bool TfrmCH341_SPI_PN532::ReadNdef(AnsiString &out)
{
	std::vector<unsigned char> data;		// user memory, starting at page 4
	const unsigned int CAP = 924;			// covers NTAG216; bounds read time

	if (!ReadNtagBytes(data, 16, CAP))		// enough for the TLV header area
		return false;

	// Walk the Type-Length-Value structure to find the NDEF message TLV (0x03).
	unsigned int pos = 0, msgStart = 0, msgLen = 0;
	bool found = false;
	while (pos < data.size())
	{
		unsigned char t = data[pos];
		if (t == 0x00) { pos++; continue; }	// NULL TLV (padding)
		if (t == 0xFE) break;				// terminator TLV

		if (pos + 1 >= data.size() && !ReadNtagBytes(data, pos + 2, CAP)) break;
		unsigned int len = data[pos + 1];
		unsigned int hdr = 2;
		if (len == 0xFF)					// 3-byte length form
		{
			if (pos + 3 >= data.size() && !ReadNtagBytes(data, pos + 4, CAP)) break;
			len = ((unsigned int)data[pos + 2] << 8) | data[pos + 3];
			hdr = 4;
		}
		if (t == 0x03) { msgStart = pos + hdr; msgLen = len; found = true; break; }
		pos += hdr + len;					// skip other TLVs (lock/memory control)
	}
	if (!found || msgLen == 0)
		return false;

	if (!ReadNtagBytes(data, msgStart + msgLen, CAP))
	{
		// Truncated read: parse as much of the message as was actually read.
		if (data.size() <= msgStart)
			return false;
		if (msgStart + msgLen > data.size())
			msgLen = data.size() - msgStart;
	}

	ParseNdefMessage(&data[msgStart], msgLen, out);
	return true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_SPI_PN532::btnReadNdefClick(TObject *Sender)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	if (!reading)
	{
		lblStatus->Caption = "Click 'Init + start reading' first";
		return;
	}

	uint8_t uid[PN532_UID_MAX_LEN] = { 0 };
	uint8_t uidLength = 0;
	if (!pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 1000))
	{
		memo->Lines->Text = "No card found";
		return;
	}

	uint16_t atqa = pn532.getLastAtqa();
	uint8_t sak = pn532.getLastSak();
	if (!(sak == 0x00 && atqa == 0x0044))
	{
		memo->Lines->Text = "NDEF read here supports NTAG / Ultralight tags only";
		return;
	}

	AnsiString out = "NDEF content:\n";
	if (ReadNdef(out))
		memo->Lines->Text = out;
	else
		memo->Lines->Text = "No NDEF message found (tag not NDEF-formatted or empty)";
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341_SPI_PN532::tmrAutoReadTimer(TObject *Sender)
{
	tmrAutoRead->Enabled = false;
	if (reading)
		Read();
	// Stop may have been clicked while Read() was in progress
	if (reading)
		tmrAutoRead->Enabled = true;
}
//---------------------------------------------------------------------------

