//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341MemProg.h"
#include "CH341A.h"
#include "TabManager.h"
#include <vector>
#include <utility>
#include <stdint.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341MemProg *frmCH341MemProg;

REGISTER_TOOL(TfrmCH341MemProg, frmCH341MemProg, 1u << ToolGroupMemory)

namespace
{
	enum DeviceIndex
	{
		DeviceSpiFlash = 0,
		DeviceSpiEeprom,
		DeviceI2cEeprom,
		DeviceMicrowire,
		DeviceAt45
	};

	struct SpiSize
	{
		const char *text;
		unsigned long size;
	};

	const SpiSize spiSizes[] = {
		{ "64 KB (512 Kbit)", 64UL * 1024 },
		{ "128 KB (1 Mbit)", 128UL * 1024 },
		{ "256 KB (2 Mbit)", 256UL * 1024 },
		{ "512 KB (4 Mbit)", 512UL * 1024 },
		{ "1 MB (8 Mbit)", 1024UL * 1024 },
		{ "2 MB (16 Mbit)", 2UL * 1024 * 1024 },
		{ "4 MB (32 Mbit)", 4UL * 1024 * 1024 },
		{ "8 MB (64 Mbit)", 8UL * 1024 * 1024 },
		{ "16 MB (128 Mbit)", 16UL * 1024 * 1024 },
		{ "32 MB (256 Mbit, 4-byte addr)", 32UL * 1024 * 1024 }
	};
	const unsigned int spiSizesCount = sizeof(spiSizes) / sizeof(spiSizes[0]);
	const unsigned int SPI_SIZE_DEFAULT_INDEX = 5;	// 2 MB

	const unsigned int spiPageSizes[] = { 256, 128, 64, 32, 16 };
	const unsigned int spiPageSizesCount = sizeof(spiPageSizes) / sizeof(spiPageSizes[0]);
}

/** Adapts MemProg progress/cancel hooks to the form */
class UiProgress : public MemProg::Progress
{
private:
	TfrmCH341MemProg *frm;
	DWORD lastUpdate;
public:
	UiProgress(TfrmCH341MemProg *frm): frm(frm), lastUpdate(0) {}

	virtual void Report(unsigned long done, unsigned long total)
	{
		const DWORD now = GetTickCount();
		if (done < total && (now - lastUpdate) < 50)
			return;
		lastUpdate = now;
		if (total == 0)
			frm->progressBar->Position = 0;
		else
			frm->progressBar->Position = static_cast<int>((static_cast<double>(done) * 1000.0) / total);
	}

	virtual bool IsCancelled(void)
	{
		Application->ProcessMessages();
		return frm->cancelRequested || Application->Terminated;	// do not keep the application waiting
	}
};

//---------------------------------------------------------------------------
__fastcall TfrmCH341MemProg::TfrmCH341MemProg(TComponent* Owner)
	: TForm(Owner),
	busy(false),
	imageDevice(-1),
	cancelRequested(false),
	progress(new UiProgress(this))
{

	hexFrame = new TframeHexEditor(this);
	hexFrame->Parent = pnlHex;
	hexFrame->Align = alClient;

	progressBar->Min = 0;
	progressBar->Max = 1000;

	pcDevice->ActivePage = tsI2cEeprom;	// default page, regardless of ActivePage saved in the dfm

	cbEraseMode->Items->Add("None (chip is blank)");
	cbEraseMode->Items->Add("Erase whole chip");
	cbEraseMode->Items->Add("Erase 4 KB sectors (only written area)");
	cbEraseMode->Items->Add("Write only changed data (read first)");
	cbEraseMode->ItemIndex = 1;

	cbProgMode->Items->Add("Page program");
	cbProgMode->Items->Add("SST AAI word (0xAD)");
	cbProgMode->Items->Add("SST AAI byte (0xAF)");
	cbProgMode->ItemIndex = 0;

	for (unsigned int i=0; i<spiSizesCount; i++)
		cbSpiSize->Items->Add(spiSizes[i].text);
	cbSpiSize->ItemIndex = SPI_SIZE_DEFAULT_INDEX;

	for (unsigned int i=0; i<spiPageSizesCount; i++)
		cbSpiPage->Items->Add(IntToStr(static_cast<int>(spiPageSizes[i])));
	cbSpiPage->ItemIndex = 0;

	for (unsigned int i=0; i<spiPageSizesCount; i++)
		cbSpiEePage->Items->Add(IntToStr(static_cast<int>(spiPageSizes[i])));

	unsigned int count;
	const MemProg::SpiEepromType *spiEeTypes = MemProg::GetSpiEepromTypes(count);
	for (unsigned int i=0; i<count; i++)
		cbSpiEeType->Items->Add(spiEeTypes[i].name);
	cbSpiEeType->ItemIndex = 9;	// 25x512
	cbSpiEeTypeChange(NULL);

	const MemProg::MicrowireType *mwTypes = MemProg::GetMicrowireTypes(count);
	for (unsigned int i=0; i<count; i++)
		cbMwType->Items->Add(mwTypes[i].name);
	cbMwType->ItemIndex = 0;
	cbMwOrg->Items->Add("x16 (ORG high, 16-bit words)");
	cbMwOrg->Items->Add("x8 (ORG low, bytes)");
	cbMwOrg->ItemIndex = 0;

	const MemProg::I2cEepromType *types = MemProg::GetI2cEepromTypes(count);
	for (unsigned int i=0; i<count; i++)
	{
		AnsiString text;
		text.sprintf("%s (%lu bytes, page %u)", types[i].name, types[i].size, types[i].pageSize);
		cbI2cType->Items->Add(text);
	}
	cbI2cType->ItemIndex = 1;	// 24C02

	cbI2cAddr->Items->Add("Auto");
	for (unsigned int i=0x50; i<=0x57; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X", i);
		cbI2cAddr->Items->Add(text);
	}
	cbI2cAddr->ItemIndex = 0;

	pcDeviceChange(NULL);
}
//---------------------------------------------------------------------------
__fastcall TfrmCH341MemProg::~TfrmCH341MemProg(void)
{
	delete progress;
}
//---------------------------------------------------------------------------
int TfrmCH341MemProg::GetDevice(void) const
{
	// by sheet, not by page index: the tabs can be reordered in the form designer
	TTabSheet *page = pcDevice->ActivePage;
	if (page == tsSpiFlash)
		return DeviceSpiFlash;
	if (page == tsSpiEeprom)
		return DeviceSpiEeprom;
	if (page == tsI2cEeprom)
		return DeviceI2cEeprom;
	if (page == tsMicrowire)
		return DeviceMicrowire;
	return DeviceAt45;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::pcDeviceChange(TObject *Sender)
{
	const int device = GetDevice();
	btnErase->Caption = (device == DeviceSpiEeprom || device == DeviceI2cEeprom) ? "Fill with 0xFF" : "Erase chip";
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::cbSpiEeTypeChange(TObject *Sender)
{
	unsigned int count;
	const MemProg::SpiEepromType *types = MemProg::GetSpiEepromTypes(count);
	if (cbSpiEeType->ItemIndex < 0)
		return;
	const unsigned int page = types[cbSpiEeType->ItemIndex].pageSize;
	for (unsigned int i=0; i<spiPageSizesCount; i++)
	{
		if (spiPageSizes[i] == page)
			cbSpiEePage->ItemIndex = static_cast<int>(i);
	}
}
//---------------------------------------------------------------------------
bool TfrmCH341MemProg::CheckOpened(void)
{
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
AnsiString TfrmCH341MemProg::DescribeI2cScan(const MemProg::I2cScanResult &scan)
{
	if (scan.ackMask == 0)
		return "no device answers at 0x50..0x57";
	AnsiString text = "ACK at";
	for (unsigned int i=0; i<8; i++)
	{
		if (scan.ackMask & (1u << i))
			text.cat_printf(" 0x%02X", 0x50 + i);
	}
	return text;
}
//---------------------------------------------------------------------------
void TfrmCH341MemProg::ApplySfdp(const MemProg::SfdpInfo &info)
{
	AnsiString text;
	text.sprintf("SFDP %u.%u: %lu KB", info.major, info.minor, info.size / 1024);
	if (info.pageSize != 0)
		text.cat_printf(", page %u B", info.pageSize);
	text += (info.addrMode == 0) ? ", 3-byte address" : ((info.addrMode == 1) ? ", 3 or 4-byte address" : ", 4-byte address");
	Log(text);

	text = "Erase:";
	for (int i=0; i<4; i++)
	{
		if (info.eraseTypes[i].size == 0)
			continue;
		if (info.eraseTypes[i].size >= 1024)
			text.cat_printf(" %lu KB", info.eraseTypes[i].size / 1024);
		else
			text.cat_printf(" %lu B", info.eraseTypes[i].size);
		text.cat_printf(" (0x%02X)", info.eraseTypes[i].opcode);
	}
	Log(text);

	bool found = false;
	for (unsigned int i=0; i<spiSizesCount; i++)
	{
		if (spiSizes[i].size == info.size)
		{
			cbSpiSize->ItemIndex = static_cast<int>(i);
			found = true;
			break;
		}
	}
	if (!found)
		Log("Size reported by the chip is not in the Size list, select the closest one manually");

	if (info.pageSize != 0)
	{
		for (unsigned int i=0; i<spiPageSizesCount; i++)
		{
			if (spiPageSizes[i] == info.pageSize)
			{
				cbSpiPage->ItemIndex = static_cast<int>(i);
				break;
			}
		}
	}
	if (!info.has4kErase || info.erase4kOpcode != 0x20)
		Log("No 4 KB erase with opcode 0x20 on this chip: \"Erase 4 KB sectors\" will not work, use chip erase");
}
//---------------------------------------------------------------------------
bool TfrmCH341MemProg::CheckPresent(void)
{
	if (GetDevice() != DeviceI2cEeprom)
		return true;

	const int status = i2cEeprom.Probe();
	if (status == MemProg::StatusOk)
		return true;

	AnsiString text = AnsiString("I2C EEPROM: ") + MemProg::StatusText(status);
	if (status == MemProg::StatusWrongDevice)
		text += " - " + DescribeI2cScan(i2cEeprom.GetScan()) + ", use Detect";
	Log(text + ", aborting");
	return false;
}
//---------------------------------------------------------------------------
unsigned long TfrmCH341MemProg::Configure(void)
{
	switch (GetDevice())
	{
	case DeviceSpiFlash:
		if (cbSpiSize->ItemIndex < 0 || cbSpiPage->ItemIndex < 0)
			return 0;
		spi.Configure(spiSizes[cbSpiSize->ItemIndex].size, spiPageSizes[cbSpiPage->ItemIndex]);
		spi.SetProgramMode(static_cast<MemProg::SpiFlash::ProgramMode>(cbProgMode->ItemIndex));
		return spi.GetSize();
	case DeviceSpiEeprom:
	{
		if (cbSpiEeType->ItemIndex < 0 || cbSpiEePage->ItemIndex < 0)
			return 0;
		unsigned int count;
		const MemProg::SpiEepromType *types = MemProg::GetSpiEepromTypes(count);
		const MemProg::SpiEepromType &type = types[cbSpiEeType->ItemIndex];
		spi.Configure(type.size, spiPageSizes[cbSpiEePage->ItemIndex], type.addrBytes);
		spi.SetProgramMode(MemProg::SpiFlash::ProgramPage);
		return spi.GetSize();
	}
	case DeviceMicrowire:
	{
		if (cbMwType->ItemIndex < 0 || cbMwOrg->ItemIndex < 0)
			return 0;
		unsigned int count;
		const MemProg::MicrowireType *types = MemProg::GetMicrowireTypes(count);
		microwire.Configure(&types[cbMwType->ItemIndex], cbMwOrg->ItemIndex == 0);
		return microwire.GetSize();
	}
	case DeviceAt45:
	{
		int status = at45.Detect();
		if (status != MemProg::StatusOk)
		{
			lblAt45Info->Caption = "not detected";
			Log(AnsiString("AT45: ") + (status == MemProg::StatusInvalidArgs ? "unsupported density" : MemProg::StatusText(status)));
			return 0;
		}
		AnsiString text;
		text.sprintf("%u pages x %u B = %lu B (%s page size)", at45.GetPageCount(), at45.GetPageSize(), at45.GetSize(),
			at45.IsPowerOfTwoPage() ? "binary" : "standard");
		lblAt45Info->Caption = text;
		return at45.GetSize();
	}
	default:
		break;
	}

	if (cbI2cType->ItemIndex < 0 || cbI2cAddr->ItemIndex < 0)
		return 0;
	unsigned int count;
	const MemProg::I2cEepromType *types = MemProg::GetI2cEepromTypes(count);
	const bool autoAddress = (cbI2cAddr->ItemIndex == 0);
	i2cEeprom.Configure(&types[cbI2cType->ItemIndex], static_cast<uint8_t>(0x50 + cbI2cAddr->ItemIndex - 1), autoAddress);
	return i2cEeprom.GetSize();
}
//---------------------------------------------------------------------------
void TfrmCH341MemProg::SetBusy(bool state)
{
	busy = state;
	cancelRequested = false;
	pcDevice->Enabled = !state;
	chkVerifyAfterWrite->Enabled = !state;
	btnRead->Enabled = !state;
	btnWrite->Enabled = !state;
	btnVerify->Enabled = !state;
	btnErase->Enabled = !state;
	btnCancel->Enabled = state;
	if (!state)
		progressBar->Position = 0;
}
//---------------------------------------------------------------------------
void TfrmCH341MemProg::Log(const AnsiString &text)
{
	memoLog->Lines->Add(text);
}
//---------------------------------------------------------------------------
void TfrmCH341MemProg::LogResult(const char *operation, int status, DWORD startTick)
{
	AnsiString text;
	const DWORD ms = GetTickCount() - startTick;
	if (status == MemProg::StatusOk)
		text.sprintf("%s: OK (%.1f s)", operation, ms / 1000.0);
	else
		text.sprintf("%s: %s", operation, MemProg::StatusText(status));
	Log(text);
}
//---------------------------------------------------------------------------
bool TfrmCH341MemProg::GetEditorData(unsigned long chipSize, std::vector<uint8_t> &data)
{
	if (!hexFrame->HasData())
	{
		MessageBox(this->Handle, "No data: read the chip or open a file in the editor first", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return false;
	}
	data = hexFrame->GetData();
	if (data.size() > chipSize)
	{
		AnsiString text;
		text.sprintf("Data in the editor is %u bytes, selected chip has %lu bytes", data.size(), chipSize);
		MessageBox(this->Handle, text.c_str(), this->Caption.c_str(), MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
int TfrmCH341MemProg::DoRead(uint8_t *data, unsigned long size)
{
	switch (GetDevice())
	{
	case DeviceSpiFlash:
	case DeviceSpiEeprom:
		return spi.Read(0, data, size, progress);
	case DeviceMicrowire:
		return microwire.Read(0, data, size, progress);
	case DeviceAt45:
		return at45.Read(0, data, size, progress);
	default:
		return i2cEeprom.Read(0, data, size, progress);
	}
}
//---------------------------------------------------------------------------
int TfrmCH341MemProg::DoWrite(uint32_t addr, const uint8_t *data, unsigned long size, bool skipBlank)
{
	switch (GetDevice())
	{
	case DeviceSpiFlash:
	case DeviceSpiEeprom:
		return spi.Write(addr, data, size, skipBlank, progress);
	case DeviceMicrowire:
		return microwire.Write(addr, data, size, progress);
	case DeviceAt45:
		return at45.Write(addr, data, size, progress);
	default:
		return i2cEeprom.Write(addr, data, size, skipBlank, progress);
	}
}
//---------------------------------------------------------------------------
int TfrmCH341MemProg::WriteEdited(const std::vector<uint8_t> &data)
{
	const std::vector<uint8_t> &orig = hexFrame->GetOriginal();
	if (orig.size() != data.size())
		return MemProg::StatusInvalidArgs;

	if (GetDevice() == DeviceSpiFlash)
	{
		// the chip is known to hold the original data, so no read is needed except around the range
		const int status = spi.WriteChanged(0, &data[0], data.size(), progress, &orig[0]);
		if (status == MemProg::StatusOk)
		{
			AnsiString text;
			text.sprintf("Erased %lu sector(s), programmed %lu page(s)", spi.GetLastErasedSectors(), spi.GetLastProgrammedPages());
			Log(text);
		}
		return status;
	}

	// other chips overwrite data in place: write the runs of edited bytes, widened to what the chip writes at once
	size_t granularity = 1;
	if (GetDevice() == DeviceAt45)
		granularity = at45.GetPageSize();
	else if (GetDevice() == DeviceMicrowire && cbMwOrg->ItemIndex == 0)
		granularity = 2;

	std::vector<std::pair<size_t, size_t> > ranges;	// [begin, end)
	size_t i = 0;
	while (i < data.size())
	{
		if (data[i] == orig[i])
		{
			i++;
			continue;
		}
		size_t begin = i;
		while (i < data.size() && data[i] != orig[i])
			i++;
		size_t end = i;
		begin -= begin % granularity;
		end = ((end + granularity - 1) / granularity) * granularity;
		if (end > data.size())
			end = data.size();
		if (!ranges.empty() && begin <= ranges.back().second)
			ranges.back().second = (end > ranges.back().second) ? end : ranges.back().second;
		else
			ranges.push_back(std::make_pair(begin, end));
	}

	size_t total = 0;
	for (size_t r=0; r<ranges.size(); r++)
	{
		const int status = DoWrite(static_cast<uint32_t>(ranges[r].first), &data[ranges[r].first],
			ranges[r].second - ranges[r].first, false);
		if (status != MemProg::StatusOk)
			return status;
		total += ranges[r].second - ranges[r].first;
	}
	AnsiString text;
	text.sprintf("Wrote %u range(s), %u byte(s)", ranges.size(), total);
	Log(text);
	return MemProg::StatusOk;
}
//---------------------------------------------------------------------------
int TfrmCH341MemProg::DoVerify(const uint8_t *data, unsigned long size, uint32_t &mismatchAddr)
{
	switch (GetDevice())
	{
	case DeviceSpiFlash:
	case DeviceSpiEeprom:
		return spi.Verify(0, data, size, mismatchAddr, progress);
	case DeviceMicrowire:
		return microwire.Verify(0, data, size, mismatchAddr, progress);
	case DeviceAt45:
		return at45.Verify(0, data, size, mismatchAddr, progress);
	default:
		return i2cEeprom.Verify(0, data, size, mismatchAddr, progress);
	}
}
//---------------------------------------------------------------------------
int TfrmCH341MemProg::DoErase(void)
{
	switch (GetDevice())
	{
	case DeviceSpiFlash:
		return spi.EraseChip(progress);
	case DeviceSpiEeprom:
		return spi.FillBlank(progress);
	case DeviceMicrowire:
		return microwire.Erase(progress);
	case DeviceAt45:
		return at45.EraseChip(progress);
	default:
		return i2cEeprom.Erase(progress);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnReadIdClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	if (Configure() == 0)
		return;

	MemProg::SpiFlashId id;
	int status = spi.ReadId(id);
	if (status == MemProg::StatusNoDevice)
	{
		Log("JEDEC ID: no response (check wiring/power, chip must be in SPI mode)");
		return;
	}
	if (status != MemProg::StatusOk)
	{
		Log(AnsiString("JEDEC ID: ") + MemProg::StatusText(status));
		return;
	}

	AnsiString text;
	text.sprintf("JEDEC ID: %02X %02X %02X - %s", id.manufacturer, id.memType, id.capacity, id.ManufacturerName());
	const unsigned long guess = id.GuessSize();
	if (guess != 0)
	{
		text.cat_printf(", capacity ~%lu KB", guess / 1024);
		for (unsigned int i=0; i<spiSizesCount; i++)
		{
			if (spiSizes[i].size == guess)
			{
				cbSpiSize->ItemIndex = static_cast<int>(i);
				break;
			}
		}
	}
	Log(text);

	MemProg::SfdpInfo sfdp;
	status = spi.ReadSfdp(sfdp);
	if (status == MemProg::StatusOk)
		ApplySfdp(sfdp);
	else if (status == MemProg::StatusNoDevice)
		Log(guess != 0 ? "SFDP: not supported by this chip, size taken from the ID" : "SFDP: not supported by this chip");
	else
		Log(AnsiString("SFDP: ") + MemProg::StatusText(status));

	uint8_t sr;
	if (spi.ReadStatus(sr) == MemProg::StatusOk)
	{
		text.sprintf("Status register 1: 0x%02X", sr);
		if (sr & 0x1C)
			text += " (block protection active, use \"Unprotect\" before write/erase)";
		Log(text);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnUnprotectClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	if (Configure() == 0)
		return;
	const DWORD start = GetTickCount();
	int status = spi.Unprotect();
	LogResult("Unprotect", status, start);
	uint8_t sr;
	if (spi.ReadStatus(sr) == MemProg::StatusOk)
	{
		AnsiString text;
		text.sprintf("Status register 1: 0x%02X", sr);
		Log(text);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnAt45DetectClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	if (Configure() == 0)
		return;
	uint8_t man, dev1, dev2;
	at45.GetId(man, dev1, dev2);
	AnsiString text;
	text.sprintf("AT45 ID: %02X %02X %02X, %s", man, dev1, dev2, lblAt45Info->Caption.c_str());
	Log(text);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnAt45UnprotectClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	if (Configure() == 0)
		return;
	const DWORD start = GetTickCount();
	int status = at45.DisableProtection();
	LogResult("Disable sector protection", status, start);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnProbeClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;

	MemProg::I2cScanResult scan;
	int status = MemProg::I2cEeprom::Scan(scan);
	if (status != MemProg::StatusOk)
	{
		Log(AnsiString("I2C EEPROM: ") + MemProg::StatusText(status));
		return;
	}
	Log("I2C EEPROM: " + DescribeI2cScan(scan));
	if (scan.ackMask == 0 || !scan.valid)
	{
		if (scan.ackMask != 0)
			Log("ACKing addresses do not form a single EEPROM (several devices, or not an EEPROM)");
		return;
	}

	unsigned int count;
	const MemProg::I2cEepromType *types = MemProg::GetI2cEepromTypes(count);
	if (scan.blocks == 1)
	{
		Log("Single address: 24C01/24C02 or 24C32 and bigger. Chip size can not be read from the chip, select the type.");
		return;
	}

	// 2, 4 or 8 addresses: the number of address bits carried in the device address tells the family
	AnsiString names;
	int first = -1;
	bool selectedFits = false;
	for (unsigned int i=0; i<count; i++)
	{
		if ((1u << types[i].addrBitsInDev) != scan.blocks)
			continue;
		if (first < 0)
			first = static_cast<int>(i);
		if (static_cast<int>(i) == cbI2cType->ItemIndex)
			selectedFits = true;
		if (names.Length() > 0)
			names += " or ";
		names += types[i].name;
	}
	if (first < 0)
		return;
	if (!selectedFits)
		cbI2cType->ItemIndex = first;
	Log("Looks like " + names + ", selected " + AnsiString(types[cbI2cType->ItemIndex].name));
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnReadClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	const unsigned long size = Configure();
	if (size == 0)
		return;
	if (!CheckPresent())
		return;
	if (hexFrame->IsModified())
	{
		if (MessageBox(this->Handle, "Data in the editor was changed. Replace it with the chip contents?",
			this->Caption.c_str(), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
			return;
	}

	std::vector<uint8_t> data(size);
	SetBusy(true);
	const DWORD start = GetTickCount();
	int status = DoRead(&data[0], size);
	SetBusy(false);
	LogResult("Read", status, start);
	if (status != MemProg::StatusOk)
		return;

	hexFrame->SetData(data, "chip " + pcDevice->ActivePage->Caption, true);
	imageDevice = GetDevice();
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnWriteClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	const unsigned long size = Configure();
	if (size == 0)
		return;
	if (!CheckPresent())
		return;

	std::vector<uint8_t> data;
	if (!GetEditorData(size, data))
		return;

	if (GetDevice() == DeviceAt45 && (data.size() % at45.GetPageSize()) != 0)
	{
		AnsiString text;
		text.sprintf("File size (%u bytes) must be a multiple of the page size (%u bytes)", data.size(), at45.GetPageSize());
		MessageBox(this->Handle, text.c_str(), this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	const bool onlyEdited = chkWriteEdited->Checked;
	if (onlyEdited && (!hexFrame->IsChipImage() || imageDevice != GetDevice() || data.size() != size))
	{
		MessageBox(this->Handle, "\"Write only edited\" needs the whole chip contents read from this kind of chip (and not "
			"replaced by a file): it writes just what was changed in the editor since then.", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	AnsiString question;
	question.sprintf("Write %u bytes from the editor to %s?", data.size(), pcDevice->ActivePage->Caption.c_str());
	if (onlyEdited)
		question.sprintf("Write the edited parts of the data to %s?", pcDevice->ActivePage->Caption.c_str());
	if (MessageBox(this->Handle, question.c_str(), this->Caption.c_str(), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
		return;

	SetBusy(true);
	int status = MemProg::StatusOk;
	bool skipBlank = false;
	if (onlyEdited)
	{
		// no erase mode needed: WriteEdited decides itself
	}
	else if (GetDevice() == DeviceSpiFlash && cbEraseMode->ItemIndex == 1)
	{
		DWORD start = GetTickCount();
		Log("Erasing chip...");
		status = spi.EraseChip(progress);
		LogResult("Erase", status, start);
		skipBlank = true;	// erased, so there is no need to program 0xFF
	}
	else if (GetDevice() == DeviceSpiFlash && cbEraseMode->ItemIndex == 2)
	{
		DWORD start = GetTickCount();
		Log("Erasing sectors...");
		status = spi.EraseSectors(0, data.size(), progress);
		LogResult("Erase", status, start);
		skipBlank = true;
	}
	if (onlyEdited)
	{
		const DWORD start = GetTickCount();
		status = WriteEdited(data);
		LogResult("Write edited", status, start);
	}
	else if (GetDevice() == DeviceSpiFlash && cbEraseMode->ItemIndex == 3)
	{
		const DWORD start = GetTickCount();
		status = spi.WriteChanged(0, &data[0], data.size(), progress);
		LogResult("Write changed", status, start);
		if (status == MemProg::StatusOk)
		{
			AnsiString text;
			text.sprintf("Erased %lu sector(s), programmed %lu page(s)", spi.GetLastErasedSectors(), spi.GetLastProgrammedPages());
			Log(text);
		}
	}
	else if (status == MemProg::StatusOk)
	{
		const DWORD start = GetTickCount();
		status = DoWrite(0, &data[0], data.size(), skipBlank);
		LogResult("Write", status, start);
	}
	if (status == MemProg::StatusOk && chkVerifyAfterWrite->Checked)
	{
		const DWORD start = GetTickCount();
		uint32_t mismatchAddr = 0;
		status = DoVerify(&data[0], data.size(), mismatchAddr);
		LogResult("Verify", status, start);
		if (status == MemProg::StatusVerifyFailed)
		{
			AnsiString text;
			text.sprintf("First mismatch at address 0x%06X", static_cast<unsigned int>(mismatchAddr));
			Log(text);
		}
	}
	if (status == MemProg::StatusOk)
	{
		// the chip now holds the editor contents: later edits are relative to this (only if they cover the whole chip)
		hexFrame->MarkClean(data.size() == size);
		imageDevice = GetDevice();
	}
	SetBusy(false);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnVerifyClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	const unsigned long size = Configure();
	if (size == 0)
		return;
	if (!CheckPresent())
		return;

	std::vector<uint8_t> data;
	if (!GetEditorData(size, data))
		return;

	SetBusy(true);
	const DWORD start = GetTickCount();
	uint32_t mismatchAddr = 0;
	int status = DoVerify(&data[0], data.size(), mismatchAddr);
	SetBusy(false);
	LogResult("Verify", status, start);
	if (status == MemProg::StatusVerifyFailed)
	{
		AnsiString text;
		text.sprintf("First mismatch at address 0x%06X", static_cast<unsigned int>(mismatchAddr));
		Log(text);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnEraseClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	if (Configure() == 0)
		return;
	if (!CheckPresent())
		return;
	if (MessageBox(this->Handle, "Erase whole chip?", this->Caption.c_str(), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
		return;

	SetBusy(true);
	const DWORD start = GetTickCount();
	int status = DoErase();
	SetBusy(false);
	LogResult("Erase", status, start);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341MemProg::btnCancelClick(TObject *Sender)
{
	cancelRequested = true;
}
//---------------------------------------------------------------------------
