//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341PN532History.h"
#include "PN532.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/ScopedBool.h"
#include "common/bin2str.h"
#include "Log.h"
#include <assert.h>
#include <stdio.h>
#include <fstream>
#include <json/json.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341PN532History *frmCH341PN532History;
//---------------------------------------------------------------------------

namespace
{
enum { COL_TIME = 0, COL_UID = 1, COL_TYPE = 2, COL_SEEN = 3, COL_LABEL = 4 };
}

__fastcall TfrmCH341PN532History::TfrmCH341PN532History(TComponent* Owner)
	: TForm(Owner),
	pn532(PN532::INTERFACE_SPI),
	reading(false),
	busy(false)
{
	TabManager::Instance().Register(this, 1u << ToolGroupMisc);

	grdLog->ColCount = 5;
	grdLog->FixedRows = 1;
	grdLog->FixedCols = 0;
	grdLog->RowCount = 2;			// header + one (blank) data row
	grdLog->Cells[COL_TIME][0] = "Last seen";
	grdLog->Cells[COL_UID][0] = "UID";
	grdLog->Cells[COL_TYPE][0] = "Type";
	grdLog->Cells[COL_SEEN][0] = "Seen";
	grdLog->Cells[COL_LABEL][0] = "Label (double-click to edit)";
	grdLog->ColWidths[COL_TIME] = 120;
	grdLog->ColWidths[COL_UID] = 110;
	grdLog->ColWidths[COL_TYPE] = 210;
	grdLog->ColWidths[COL_SEEN] = 40;
	grdLog->ColWidths[COL_LABEL] = 160;

	LoadHistory();
	RefreshGrid();
}
//---------------------------------------------------------------------------

__fastcall TfrmCH341PN532History::~TfrmCH341PN532History(void)
{
}
//---------------------------------------------------------------------------

AnsiString TfrmCH341PN532History::HistoryFilePath(void)
{
	return ExtractFilePath(Application->ExeName) + "pn532_tag_history.json";
}
//---------------------------------------------------------------------------

void TfrmCH341PN532History::LoadHistory(void)
{
	Json::Value root;
	Json::Reader reader;
	try
	{
		std::ifstream ifs(HistoryFilePath().c_str());
		if (!ifs)
			return;		// no history file yet - nothing to load
		std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		if (!reader.parse(content, root))
			return;
	}
	catch (...)
	{
		return;
	}

	const Json::Value &tags = root["tags"];
	for (unsigned int i = 0; i < tags.size(); i++)
	{
		const Json::Value &t = tags[i];
		TagLogEntry entry;
		entry.uid = t.get("uid", "").asCString();
		entry.type = t.get("type", "").asCString();
		entry.label = t.get("label", "").asCString();
		entry.firstSeen = t.get("firstSeen", "").asCString();
		entry.lastSeen = t.get("lastSeen", "").asCString();
		entry.count = t.get("count", 0).asInt();
		if (entry.uid != "")
			tagLog.push_back(entry);
	}
}
//---------------------------------------------------------------------------

void TfrmCH341PN532History::SaveHistory(void)
{
	if (tagLog.empty())
		return;		// do not write an empty history file (or clobber an old one)

	Json::Value root;
	Json::Value &tags = root["tags"];
	for (size_t i = 0; i < tagLog.size(); i++)
	{
		Json::Value t;
		t["uid"] = tagLog[i].uid.c_str();
		t["type"] = tagLog[i].type.c_str();
		t["label"] = tagLog[i].label.c_str();
		t["firstSeen"] = tagLog[i].firstSeen.c_str();
		t["lastSeen"] = tagLog[i].lastSeen.c_str();
		t["count"] = tagLog[i].count;
		tags.append(t);
	}

	Json::StyledWriter writer;
	std::string out = writer.write(root);
	try
	{
		std::ofstream ofs(HistoryFilePath().c_str());
		ofs << out;
		ofs.close();
	}
	catch (...)
	{
		LOG("PN532 history: failed to write %s\n", HistoryFilePath().c_str());
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341PN532History::btnStartClick(TObject *Sender)
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
	LOG("PN532 history (SPI): begin\n");
	if (pn532.begin() != 0)
	{
		lblStatus->Caption = "Failed to init / wake up PN532";
		return;
	}

	uint32_t versiondata = pn532.getFirmwareVersion();
	if (!versiondata)
	{
		lblStatus->Caption = "Failed to get PN532 firmware version!";
		return;
	}
	AnsiString version;
	version.sprintf("Found PN5%02X, version %d.%d",
		(versiondata >> 24) & 0xFF, (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);
	lblStatus->Caption = version;

	// Finite retries so InListPassiveTarget returns "0 targets" itself when no
	// tag is present (0xFF would retry forever and leave the chip busy).
	pn532.setPassiveActivationRetries(0x10);

	reading = true;
	presentUid = "";
	lblState->Caption = "Scanning...";
	btnStart->Enabled = false;
	btnStop->Enabled = true;
	tmrPoll->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341PN532History::btnStopClick(TObject *Sender)
{
	reading = false;
	tmrPoll->Enabled = false;
	lblState->Caption = "Stopped";
	btnStop->Enabled = false;
	btnStart->Enabled = true;
}
//---------------------------------------------------------------------------

void TfrmCH341PN532History::Poll(void)
{
	if (busy)
		return;
	ScopedBool guard(&busy);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t uid[PN532_UID_MAX_LEN] = { 0 };
	uint8_t uidLength = 0;
	bool success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 1000);

	if (success)
	{
		AnsiString uidStr = BufToSpaceSeparatedHexString(uid, uidLength).c_str();
		uint16_t atqa = pn532.getLastAtqa();
		uint8_t sak = pn532.getLastSak();
		AnsiString typeStr = PN532::describeIso14443aCard(atqa, sak);
		LogTag(uidStr, typeStr);
	}
	else
	{
		presentUid = "";	// tag removed: next appearance counts as a new event
	}
}
//---------------------------------------------------------------------------

void TfrmCH341PN532History::LogTag(const AnsiString &uid, const AnsiString &type)
{
	if (uid == presentUid)
		return;			// same tag still on the reader - not a new event
	presentUid = uid;

	AnsiString now = FormatDateTime("yyyy-mm-dd hh:nn:ss", Now());

	for (size_t i = 0; i < tagLog.size(); i++)
	{
		if (tagLog[i].uid == uid)
		{
			tagLog[i].count++;
			tagLog[i].lastSeen = now;
			RefreshGrid();
			return;
		}
	}

	TagLogEntry entry;
	entry.uid = uid;
	entry.type = type;
	entry.label = "";
	entry.firstSeen = now;
	entry.lastSeen = now;
	entry.count = 1;
	tagLog.push_back(entry);
	RefreshGrid();
}
//---------------------------------------------------------------------------

void TfrmCH341PN532History::RefreshGrid(void)
{
	// Row 0 is the fixed header; keep at least one data row so FixedRows < RowCount.
	grdLog->RowCount = (tagLog.size() < 1) ? 2 : static_cast<int>(tagLog.size() + 1);

	if (tagLog.empty())
	{
		for (int c = 0; c < 5; c++)
			grdLog->Cells[c][1] = "";
		return;
	}

	for (size_t i = 0; i < tagLog.size(); i++)
	{
		int row = static_cast<int>(i + 1);
		grdLog->Cells[COL_TIME][row] = tagLog[i].lastSeen;
		grdLog->Cells[COL_UID][row] = tagLog[i].uid;
		grdLog->Cells[COL_TYPE][row] = tagLog[i].type;
		grdLog->Cells[COL_SEEN][row] = IntToStr(tagLog[i].count);
		grdLog->Cells[COL_LABEL][row] = tagLog[i].label;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341PN532History::grdLogSelectCell(TObject *Sender, int ACol,
	  int ARow, bool &CanSelect)
{
	// Only the label column is editable; the rest is read-only device data.
	if (ACol == COL_LABEL && ARow >= 1 && ARow <= static_cast<int>(tagLog.size()))
		grdLog->Options = grdLog->Options << goEditing;
	else
		grdLog->Options = grdLog->Options >> goEditing;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341PN532History::grdLogSetEditText(TObject *Sender, int ACol,
	  int ARow, const AnsiString Value)
{
	if (ACol == COL_LABEL && ARow >= 1 && ARow <= static_cast<int>(tagLog.size()))
		tagLog[ARow - 1].label = Value;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341PN532History::btnClearLogClick(TObject *Sender)
{
	tagLog.clear();
	presentUid = "";
	RefreshGrid();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341PN532History::btnExportCsvClick(TObject *Sender)
{
	if (tagLog.empty())
	{
		lblStatus->Caption = "Tag log is empty - nothing to export";
		return;
	}
	if (!dlgExport->Execute())
		return;

	FILE *f = fopen(AnsiString(dlgExport->FileName).c_str(), "w");
	if (!f)
	{
		lblStatus->Caption = "Could not open file for writing";
		return;
	}
	fprintf(f, "First seen,Last seen,UID,Type,Seen count,Label\n");
	for (size_t i = 0; i < tagLog.size(); i++)
	{
		// Quote every field and double any embedded quotes (RFC 4180) so that
		// commas in a type description or label do not break the columns.
		AnsiString fields[6];
		fields[0] = tagLog[i].firstSeen;
		fields[1] = tagLog[i].lastSeen;
		fields[2] = tagLog[i].uid;
		fields[3] = tagLog[i].type;
		fields[4] = IntToStr(tagLog[i].count);
		fields[5] = tagLog[i].label;
		for (int c = 0; c < 6; c++)
		{
			AnsiString v = fields[c];
			v = StringReplace(v, "\"", "\"\"", TReplaceFlags() << rfReplaceAll);
			fprintf(f, "%s\"%s\"", c ? "," : "", v.c_str());
		}
		fprintf(f, "\n");
	}
	fclose(f);
	lblStatus->Caption = "Exported " + IntToStr((int)tagLog.size()) + " tag(s)";
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341PN532History::tmrPollTimer(TObject *Sender)
{
	tmrPoll->Enabled = false;
	if (reading)
		Poll();
	// Stop may have been clicked while Poll() was in progress
	if (reading)
		tmrPoll->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341PN532History::btnSaveHistoryClick(TObject *Sender)
{
	SaveHistory();
}
//---------------------------------------------------------------------------

