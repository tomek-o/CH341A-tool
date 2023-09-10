/** \file
 *  \brief Log display form module
 *
 *  Log display window is coupled using OnLog callback.
 *  Log callback enqueues supplied log line to private data containter.
 *  Data container size is limited - oldest log entries are discarded on
 *  overflow.
 *  OnLog callback is not coupled with VCL, so Synchronize() is not required.
 */

//---------------------------------------------------------------------------

#ifndef LogUnitH
#define LogUnitH
//---------------------------------------------------------------------------

#include "common/Mutex.h"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <deque>
#include <string>

//---------------------------------------------------------------------------
/** \brief Log display window
*/
class TfrmLog : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmrUpdate;
	TRichEdit *redMain;
	TPanel *pnlBottom;
	TButton *btnClear;
	TCheckBox *chbAutoScroll;
	TPopupMenu *pmenuRichEdit;
	TMenuItem *miCopy;
	TMenuItem *miSaveToFile;
	TSaveDialog *saveDialog;
	TCheckBox *chbLogToFile;
	void __fastcall tmrUpdateTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall btnClearClick(TObject *Sender);
	void __fastcall miCopyClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall miSaveToFileClick(TObject *Sender);
	void __fastcall chbLogToFileClick(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
	Mutex mutex;
	typedef void (__closure *CallbackClose)(void);

	struct LogEntry
	{
		TColor color;
		AnsiString asText;
	};
	std::deque<LogEntry> queDisplay;
	void AddTextToLog(bool bUseTimestamp, TColor color, AnsiString asText);

	unsigned int iMaxUiLogLines;
		
public:		// User declarations
	__fastcall TfrmLog(TComponent* Owner);
	void OnLog(char *buf);                         ///< add text to log window
	void SetLogLinesLimit(int lines) {
    	iMaxUiLogLines = lines;
	}
	CallbackClose callbackClose;                   ///< called when log window is closing
	void UpdateUi(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmLog *frmLog;
//---------------------------------------------------------------------------
#endif
