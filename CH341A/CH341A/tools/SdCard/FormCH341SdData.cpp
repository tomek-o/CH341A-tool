//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SdData.h"
#include "CH341A.h"
#include "TabManager.h"
#include <stdlib.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SdData *frmCH341SdData;

/** Adapts SdMmc progress/cancel hooks to the form */
class SdDataProgress : public MemProg::Progress
{
private:
	TfrmCH341SdData *frm;
	DWORD lastUpdate;
public:
	SdDataProgress(TfrmCH341SdData *frm): frm(frm), lastUpdate(0) {}

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
		return frm->cancelRequested || Application->Terminated;
	}
};

/** Every command and response goes to the log */
class SdDataTrace : public SdMmc::SdTrace
{
private:
	TfrmCH341SdData *frm;
public:
	SdDataTrace(TfrmCH341SdData *frm): frm(frm) {}
	virtual void Trace(const std::string &line)
	{
		frm->Log(line.c_str());
	}
};

//---------------------------------------------------------------------------
__fastcall TfrmCH341SdData::TfrmCH341SdData(TComponent* Owner)
	: TForm(Owner),
	busy(false),
	cancelRequested(false),
	progress(new SdDataProgress(this)),
	trace(new SdDataTrace(this))
{
	TabManager::Instance().Register(this, 1u << ToolGroupSdMmc);

	hexFrame = new TframeHexEditor(this);
	hexFrame->Parent = pnlHex;
	hexFrame->Align = alClient;

	progressBar->Min = 0;
	progressBar->Max = 1000;

	memoConnection->Lines->Text = SdMmc::GetConnectionText();
}
//---------------------------------------------------------------------------
__fastcall TfrmCH341SdData::~TfrmCH341SdData(void)
{
	if (SdMmc::sdCard.GetTrace() == trace)
		SdMmc::sdCard.SetTrace(NULL);
	delete trace;
	delete progress;
}
//---------------------------------------------------------------------------
bool TfrmCH341SdData::CheckOpened(void)
{
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool TfrmCH341SdData::CheckInitialized(void)
{
	if (!SdMmc::sdCard.IsInitialized())
	{
		MessageBox(this->Handle, "Initialize the card first (Init card)", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void TfrmCH341SdData::SetBusy(bool state)
{
	busy = state;
	cancelRequested = false;
	if (state)
		SdMmc::sdCard.SetTrace(trace);
	btnInit->Enabled = !state;
	btnReadSectors->Enabled = !state;
	btnWriteSectors->Enabled = !state;
	btnCancel->Enabled = state;
	if (!state)
		progressBar->Position = 0;
}
//---------------------------------------------------------------------------
void TfrmCH341SdData::Log(const AnsiString &text)
{
	memoLog->Lines->Add(text);
}
//---------------------------------------------------------------------------
void TfrmCH341SdData::LogResult(const char *operation, int status)
{
	AnsiString text;
	text.sprintf("%s: %s", operation, MemProg::StatusText(status));
	Log(text);
}
//---------------------------------------------------------------------------
bool TfrmCH341SdData::ParseNumber(const AnsiString &text, unsigned long &value)
{
	AnsiString s = text.Trim();
	if (s.IsEmpty())
		return false;
	char *end = NULL;
	value = strtoul(s.c_str(), &end, 0);	// decimal or 0x hex
	return end && *end == 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdData::btnInitClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	SetBusy(true);
	const int status = SdMmc::sdCard.Init(progress);
	SetBusy(false);
	LogResult("Init", status);
	if (status == MemProg::StatusNoDevice)
		Log("No answer to CMD0: check wiring, 3.3 V supply, pull-up on CS");
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdData::btnReadSectorsClick(TObject *Sender)
{
	if (busy || !CheckOpened() || !CheckInitialized())
		return;
	unsigned long sector, count;
	if (!ParseNumber(edSector->Text, sector) || !ParseNumber(edCount->Text, count) || count < 1 || count > 2048)
	{
		MessageBox(this->Handle, "Sector number and count (1..2048, decimal or 0x hex) are required", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	if (hexFrame->IsModified() &&
		MessageBox(this->Handle, "Data in the editor was changed. Replace it with the card contents?",
			this->Caption.c_str(), MB_ICONQUESTION | MB_OKCANCEL) != IDOK)
		return;

	std::vector<uint8_t> data(static_cast<size_t>(count) * SdMmc::SdCard::SECTOR_SIZE);
	SetBusy(true);
	const int status = SdMmc::sdCard.ReadSectors(sector, count, &data[0], progress);
	SetBusy(false);
	LogResult("Read sectors", status);
	if (status != MemProg::StatusOk)
		return;
	AnsiString name;
	name.sprintf("SD sector %lu, %lu sector(s)", sector, count);
	hexFrame->SetData(data, name);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdData::btnWriteSectorsClick(TObject *Sender)
{
	if (busy || !CheckOpened() || !CheckInitialized())
		return;
	if (!chkAllowWrite->Checked)
	{
		MessageBox(this->Handle, "Writing is locked: tick \"Allow writing\" first", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	unsigned long sector;
	if (!ParseNumber(edSector->Text, sector))
	{
		MessageBox(this->Handle, "Sector number is required (decimal or 0x hex)", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	if (!hexFrame->HasData())
	{
		MessageBox(this->Handle, "No data: read sectors or open a file in the editor first", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	const std::vector<uint8_t> data = hexFrame->GetData();
	if (data.empty() || (data.size() % SdMmc::SdCard::SECTOR_SIZE) != 0)
	{
		MessageBox(this->Handle, "Editor data must be a multiple of 512 bytes", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	const unsigned long count = static_cast<unsigned long>(data.size() / SdMmc::SdCard::SECTOR_SIZE);
	AnsiString question;
	question.sprintf("Write %lu sector(s) starting at sector %lu?\nThis overwrites data on the SdMmc::sdCard.", count, sector);
	if (MessageBox(this->Handle, question.c_str(), this->Caption.c_str(), MB_ICONWARNING | MB_OKCANCEL) != IDOK)
		return;

	SetBusy(true);
	const int status = SdMmc::sdCard.WriteSectors(sector, count, &data[0], progress);
	SetBusy(false);
	LogResult("Write sectors", status);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdData::btnCancelClick(TObject *Sender)
{
	cancelRequested = true;
}
//---------------------------------------------------------------------------
