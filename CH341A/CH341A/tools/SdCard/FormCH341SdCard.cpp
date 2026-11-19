//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SdCard.h"
#include "CH341A.h"
#include "TabManager.h"
#include <stdlib.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SdCard *frmCH341SdCard;

REGISTER_TOOL(TfrmCH341SdCard, frmCH341SdCard, 1u << ToolGroupSdMmc)

namespace
{
	struct Preset
	{
		unsigned int cmd;
		bool app;
		unsigned long arg;
		SdMmc::SdResponse type;
	};

	// order matches btnPreset0..7
	const Preset presets[] = {
		{ 0, false, 0, SdMmc::SdR1 },
		{ 8, false, 0x1AA, SdMmc::SdR3R7 },
		{ 9, false, 0, SdMmc::SdR1 },	// CSD: data block, handled separately
		{ 10, false, 0, SdMmc::SdR1 },	// CID: data block, handled separately
		{ 13, false, 0, SdMmc::SdR2 },
		{ 16, false, 512, SdMmc::SdR1 },
		{ 58, false, 0, SdMmc::SdR3R7 },
		{ 41, true, 0x40000000UL, SdMmc::SdR1 }
	};
}

/** Adapts SdMmc progress/cancel hooks to the form */
class SdCmdProgress : public MemProg::Progress
{
private:
	TfrmCH341SdCard *frm;
	DWORD lastUpdate;
public:
	SdCmdProgress(TfrmCH341SdCard *frm): frm(frm), lastUpdate(0) {}

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
class SdCmdTrace : public SdMmc::SdTrace
{
private:
	TfrmCH341SdCard *frm;
public:
	SdCmdTrace(TfrmCH341SdCard *frm): frm(frm) {}
	virtual void Trace(const std::string &line)
	{
		frm->Log(line.c_str());
	}
};

//---------------------------------------------------------------------------
__fastcall TfrmCH341SdCard::TfrmCH341SdCard(TComponent* Owner)
	: TForm(Owner),
	busy(false),
	cancelRequested(false),
	progress(new SdCmdProgress(this)),
	trace(new SdCmdTrace(this))
{

	progressBar->Min = 0;
	progressBar->Max = 1000;
	memoConnection->Lines->Text = SdMmc::GetConnectionText();

	cbResponse->Items->Add("R1 (1 byte)");
	cbResponse->Items->Add("R2 (R1 + 1 byte)");
	cbResponse->Items->Add("R3/R7 (R1 + 4 bytes)");
	cbResponse->ItemIndex = 0;
}
//---------------------------------------------------------------------------
__fastcall TfrmCH341SdCard::~TfrmCH341SdCard(void)
{
	if (SdMmc::sdCard.GetTrace() == trace)
		SdMmc::sdCard.SetTrace(NULL);
	delete trace;
	delete progress;
}
//---------------------------------------------------------------------------
bool TfrmCH341SdCard::CheckOpened(void)
{
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool TfrmCH341SdCard::CheckInitialized(void)
{
	if (!SdMmc::sdCard.IsInitialized())
	{
		MessageBox(this->Handle, "Initialize the card first (Init card)", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void TfrmCH341SdCard::SetBusy(bool state)
{
	busy = state;
	cancelRequested = false;
	if (state)
		SdMmc::sdCard.SetTrace(trace);
	btnInit->Enabled = !state;
	btnInfo->Enabled = !state;
	btnStatus->Enabled = !state;
	btnSend->Enabled = !state;
	TButton *presetButtons[] = { btnPreset0, btnPreset1, btnPreset2, btnPreset3, btnPreset4, btnPreset5, btnPreset6, btnPreset7 };
	for (int i=0; i<8; i++)
		presetButtons[i]->Enabled = !state;
	btnCancel->Enabled = state;
	if (!state)
		progressBar->Position = 0;
}
//---------------------------------------------------------------------------
void TfrmCH341SdCard::Log(const AnsiString &text)
{
	memoLog->Lines->Add(text);
}
//---------------------------------------------------------------------------
void TfrmCH341SdCard::LogResult(const char *operation, int status)
{
	AnsiString text;
	text.sprintf("%s: %s", operation, MemProg::StatusText(status));
	Log(text);
}
//---------------------------------------------------------------------------
void TfrmCH341SdCard::Info(const AnsiString &text)
{
	memoInfo->Lines->Add(text);
}
//---------------------------------------------------------------------------
bool TfrmCH341SdCard::ParseNumber(const AnsiString &text, int base, unsigned long &value)
{
	AnsiString s = text.Trim();
	if (s.IsEmpty())
		return false;
	char *end = NULL;
	value = strtoul(s.c_str(), &end, base);
	return end && *end == 0;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdCard::btnInitClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	SetBusy(true);
	const int status = SdMmc::sdCard.Init(progress);
	SetBusy(false);
	LogResult("Init", status);
	if (status == MemProg::StatusOk)
	{
		AnsiString text;
		text.sprintf("Card initialized: SD %s, %s", SdMmc::sdCard.IsVersion2() ? "v2 (or later)" : "v1 / MMC",
			SdMmc::sdCard.IsHighCapacity() ? "SDHC/SDXC (block addressing)" : "SDSC (byte addressing)");
		Info(text);
	}
	else if (status == MemProg::StatusNoDevice)
		Info("No answer to CMD0: check wiring, 3.3 V supply, pull-up on CS (see the Wiring tab)");
}
//---------------------------------------------------------------------------
void TfrmCH341SdCard::ReadCidCsd(void)
{
	SdMmc::SdCid cid;
	int status = SdMmc::sdCard.ReadCid(cid);
	LogResult("CID", status);
	if (status == MemProg::StatusOk)
		Info(SdMmc::SdCard::DescribeCid(cid).c_str());
	SdMmc::SdCsd csd;
	status = SdMmc::sdCard.ReadCsd(csd);
	LogResult("CSD", status);
	if (status == MemProg::StatusOk)
		Info(SdMmc::SdCard::DescribeCsd(csd).c_str());
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdCard::btnInfoClick(TObject *Sender)
{
	if (busy || !CheckOpened() || !CheckInitialized())
		return;
	SetBusy(true);
	uint32_t ocr;
	if (SdMmc::sdCard.ReadOcr(ocr) == MemProg::StatusOk)
		Info(AnsiString("OCR: ") + SdMmc::SdCard::DescribeOcr(ocr).c_str());
	ReadCidCsd();
	SetBusy(false);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdCard::btnStatusClick(TObject *Sender)
{
	if (busy || !CheckOpened() || !CheckInitialized())
		return;
	SetBusy(true);
	uint8_t r1 = 0, r2 = 0;
	const int status = SdMmc::sdCard.ReadStatus(r1, r2);
	SetBusy(false);
	LogResult("Status", status);
	if (status == MemProg::StatusOk)
	{
		AnsiString text;
		text.sprintf("Status R1=%02X R2=%02X", r1, r2);
		Info(text);
	}
}
//---------------------------------------------------------------------------
void TfrmCH341SdCard::SendCommand(unsigned int cmd, bool app, unsigned long arg, SdMmc::SdResponse type)
{
	SetBusy(true);
	uint8_t r1 = 0xFF;
	uint8_t extra[4] = { 0, 0, 0, 0 };
	int status;
	if (app)
		status = SdMmc::sdCard.AppCommand(static_cast<uint8_t>(cmd), arg, type, r1, extra);
	else
		status = SdMmc::sdCard.Command(static_cast<uint8_t>(cmd), arg, type, r1, extra);
	SetBusy(false);
	if (status != MemProg::StatusOk)
		LogResult("Command", status);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdCard::btnPresetClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	const int index = static_cast<TComponent*>(Sender)->Tag;
	if (index < 0 || index > 7)
		return;
	const Preset &p = presets[index];
	edCmd->Text = IntToStr(static_cast<int>(p.cmd));
	chkApp->Checked = p.app;
	edArg->Text = IntToHex(static_cast<int>(p.arg), 8);
	cbResponse->ItemIndex = static_cast<int>(p.type);
	if (p.cmd == 9 || p.cmd == 10)
	{
		if (!CheckInitialized())
			return;
		SetBusy(true);
		ReadCidCsd();
		SetBusy(false);
		return;
	}
	SendCommand(p.cmd, p.app, p.arg, p.type);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdCard::btnSendClick(TObject *Sender)
{
	if (busy || !CheckOpened())
		return;
	unsigned long cmd, arg;
	if (!ParseNumber(edCmd->Text, 10, cmd) || cmd > 63 || !ParseNumber(edArg->Text, 16, arg))
	{
		MessageBox(this->Handle, "Command must be 0..63 (decimal), argument hexadecimal", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
	const int type = cbResponse->ItemIndex;
	SendCommand(static_cast<unsigned int>(cmd), chkApp->Checked, arg,
		type == 1 ? SdMmc::SdR2 : (type == 2 ? SdMmc::SdR3R7 : SdMmc::SdR1));
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SdCard::btnCancelClick(TObject *Sender)
{
	cancelRequested = true;
}
//---------------------------------------------------------------------------
