//---------------------------------------------------------------------------

#ifndef FormCH341PN532HistoryH
#define FormCH341PN532HistoryH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>

#include "PN532.h"
#include <vector>

//---------------------------------------------------------------------------
// Standalone tool: continuously polls for ISO14443A tags over SPI and keeps a
// history of every distinct tag seen. Owns its own PN532 instance and hardware
// init/close, independent of the plain PN532 reader tools.
//---------------------------------------------------------------------------
class TfrmCH341PN532History : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmrPoll;
	TLabel *lblStatus;
	TLabel *lblState;
	TButton *btnStart;
	TButton *btnStop;
	TLabel *lblLog;
	TStringGrid *grdLog;
	TButton *btnClearLog;
	TButton *btnExportCsv;
	TSaveDialog *dlgExport;
	TButton *btnSaveHistory;
	void __fastcall tmrPollTimer(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall btnClearLogClick(TObject *Sender);
	void __fastcall btnExportCsvClick(TObject *Sender);
	void __fastcall grdLogSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall grdLogSetEditText(TObject *Sender, int ACol, int ARow, const AnsiString Value);
	void __fastcall btnSaveHistoryClick(TObject *Sender);
private:	// User declarations
	PN532 pn532;
	bool reading;
	bool busy;
	struct TagLogEntry {
		AnsiString uid;
		AnsiString type;
		AnsiString label;
		AnsiString firstSeen;
		AnsiString lastSeen;
		int count;
	};
	std::vector<TagLogEntry> tagLog;
	// UID currently on the reader; a fresh log event is only registered on the
	// absent->present transition (or when a different tag appears), so a tag
	// left sitting on the antenna is not re-counted on every poll.
	AnsiString presentUid;
	void Poll(void);
	void LogTag(const AnsiString &uid, const AnsiString &type);
	void RefreshGrid(void);
	AnsiString HistoryFilePath(void);
	void LoadHistory(void);
	void SaveHistory(void);
public:		// User declarations
	__fastcall TfrmCH341PN532History(TComponent* Owner);
	__fastcall ~TfrmCH341PN532History(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341PN532History *frmCH341PN532History;
//---------------------------------------------------------------------------
#endif
