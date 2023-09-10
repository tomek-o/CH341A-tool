/** \file
 *  \brief Log display form module
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <ClipBrd.hpp>

#include "LogUnit.h"
#include "Log.h"
#include "common/ScopedLock.h"
#include "common/Mutex.h"
#include "Settings.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmLog *frmLog;
//---------------------------------------------------------------------------
__fastcall TfrmLog::TfrmLog(TComponent* Owner)
	: TForm(Owner),
	iMaxUiLogLines(1000)
{
	callbackClose = NULL;
}
//---------------------------------------------------------------------------

void TfrmLog::OnLog(char *buf)
{
	ScopedLock<Mutex> lock(mutex);
	/** \note DO NOT use any VCL code for safety */
	AddTextToLog(false, clWindowText, buf);
}

/** Actual GUI update
 */
void __fastcall TfrmLog::tmrUpdateTimer(TObject *Sender)
{
// note: may cause freezing when opening log window after long time
//	if (!Visible)
//		return;
	ScopedLock<Mutex> lock(mutex);

	if (queDisplay.empty())
		return;

	// first save the current caret location
	int selStart = redMain->SelStart;
	int selLength = redMain->SelLength;
	// get the line number of the first visible line
	int firstLine = ::SendMessage(redMain->Handle, EM_GETFIRSTVISIBLELINE, 0 , 0);
	// disable screen redraws
	redMain->Lines->BeginUpdate();

	std::deque<LogEntry>::iterator iter;
	for(iter = queDisplay.begin(); iter != queDisplay.end(); ++iter)
	{
		redMain->SelStart = redMain->GetTextLen();
		redMain->SelLength = 0;
		redMain->SelAttributes->Color = iter->color;
		redMain->SelAttributes->Name = "Courier New";
		redMain->SelText = iter->asText;
	}
	queDisplay.clear();

	while ((unsigned int)redMain->Lines->Count > iMaxUiLogLines)
	{
		redMain->Lines->Delete(0);
	}

	// restore the caret location
	redMain->SelStart = selStart;
	redMain->SelLength = selLength;

	if (chbAutoScroll->Checked)
	{
		redMain->SelStart = redMain->GetTextLen();
		redMain->Perform( EM_SCROLLCARET, 0 , 0 );
	}
	else
	{
		// get the current first visible line,
		// compute the number of lines to scroll, and scroll...
		int currLine = ::SendMessage(redMain->Handle, EM_GETFIRSTVISIBLELINE, 0, 0);
		::SendMessage(redMain->Handle, EM_LINESCROLL, 0, firstLine - currLine);
	}
	redMain->Lines->EndUpdate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (callbackClose)
		callbackClose();	
}
//---------------------------------------------------------------------------

void TfrmLog::AddTextToLog(bool bUseTimestamp, TColor color, AnsiString asText)
{
	/** \note DO NOT use any VCL code for safety */
	struct LogEntry logEntry;
	logEntry.color = color;
	logEntry.asText = asText;
	queDisplay.push_back(logEntry);
	if (queDisplay.size() > this->iMaxUiLogLines)
		queDisplay.pop_front();
}
void __fastcall TfrmLog::FormResize(TObject *Sender)
{
	redMain->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::btnClearClick(TObject *Sender)
{
	redMain->Clear();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::miCopyClick(TObject *Sender)
{
	Clipboard()->SetTextBuf( redMain->SelText.c_str() );	
}
//---------------------------------------------------------------------------


void __fastcall TfrmLog::FormShow(TObject *Sender)
{
	UpdateUi();
}
//---------------------------------------------------------------------------

void TfrmLog::UpdateUi(void)
{
	chbLogToFile->Checked = appSettings.Logging.bLogToFile;
}

void __fastcall TfrmLog::miSaveToFileClick(TObject *Sender)
{
	AnsiString fname;
	fname = saveDialog->InitialDir +
		((saveDialog->InitialDir!="")?("\\"):("")) +
		FormatDateTime("yyyymmdd_hhnnss", Now());
	saveDialog->FileName = fname;
	if (saveDialog->Execute())
	{
        redMain->Lines->SaveToFile(saveDialog->FileName);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::chbLogToFileClick(TObject *Sender)
{
	appSettings.Logging.bLogToFile = chbLogToFile->Checked;
	if (appSettings.Logging.bLogToFile)
		CLog::Instance()->SetFile(ChangeFileExt(Application->ExeName, ".log").c_str());
	else
		CLog::Instance()->SetFile("");
}
//---------------------------------------------------------------------------

void __fastcall TfrmLog::FormKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_ESCAPE)
	{
		Close();
	}
}
//---------------------------------------------------------------------------

