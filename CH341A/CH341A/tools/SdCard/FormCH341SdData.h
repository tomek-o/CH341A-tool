//---------------------------------------------------------------------------

#ifndef FormCH341SdDataH
#define FormCH341SdDataH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "SdCard.h"
#include "FrameHexEditor.h"
//---------------------------------------------------------------------------
/** SD / MMC card in SPI mode: reading and writing sectors, data shown in the hex editor */
class TfrmCH341SdData : public TForm
{
__published:	// IDE-managed Components
	TMemo *memoConnection;
	TButton *btnInit;
	TLabel *lblSector;
	TEdit *edSector;
	TLabel *lblCount;
	TEdit *edCount;
	TButton *btnReadSectors;
	TButton *btnWriteSectors;
	TCheckBox *chkAllowWrite;
	TPanel *pnlHex;
	TProgressBar *progressBar;
	TButton *btnCancel;
	TMemo *memoLog;
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnReadSectorsClick(TObject *Sender);
	void __fastcall btnWriteSectorsClick(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
private:	// User declarations
	TframeHexEditor *hexFrame;
	bool busy;
	bool cancelRequested;
	MemProg::Progress *progress;
	SdMmc::SdTrace *trace;

	friend class SdDataProgress;
	friend class SdDataTrace;

	bool CheckOpened(void);
	bool CheckInitialized(void);
	void SetBusy(bool state);
	void Log(const AnsiString &text);
	void LogResult(const char *operation, int status);
	static bool ParseNumber(const AnsiString &text, unsigned long &value);
public:		// User declarations
	__fastcall TfrmCH341SdData(TComponent* Owner);
	__fastcall ~TfrmCH341SdData(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341SdData *frmCH341SdData;
//---------------------------------------------------------------------------
#endif
