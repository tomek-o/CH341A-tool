//---------------------------------------------------------------------------

#ifndef FormCH341SdCardH
#define FormCH341SdCardH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include "SdCard.h"
//---------------------------------------------------------------------------
/** SD / MMC card in SPI mode: initialization, card identification and raw commands */
class TfrmCH341SdCard : public TForm
{
__published:	// IDE-managed Components
	TMemo *memoConnection;
	TButton *btnInit;
	TButton *btnInfo;
	TButton *btnStatus;
	TButton *btnCancel;
	TLabel *lblPresets;
	TButton *btnPreset0;
	TButton *btnPreset1;
	TButton *btnPreset2;
	TButton *btnPreset3;
	TButton *btnPreset4;
	TButton *btnPreset5;
	TButton *btnPreset6;
	TButton *btnPreset7;
	TLabel *lblCmd;
	TEdit *edCmd;
	TCheckBox *chkApp;
	TLabel *lblArg;
	TEdit *edArg;
	TComboBox *cbResponse;
	TButton *btnSend;
	TMemo *memoInfo;
	TProgressBar *progressBar;
	TMemo *memoLog;
	TButton *btnClearLog;
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnInfoClick(TObject *Sender);
	void __fastcall btnStatusClick(TObject *Sender);
	void __fastcall btnPresetClick(TObject *Sender);
	void __fastcall btnSendClick(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall btnClearLogClick(TObject *Sender);
private:	// User declarations
	bool busy;
	bool cancelRequested;
	MemProg::Progress *progress;
	SdMmc::SdTrace *trace;

	friend class SdCmdProgress;
	friend class SdCmdTrace;

	bool CheckOpened(void);
	bool CheckInitialized(void);
	void SetBusy(bool state);
	void Log(const AnsiString &text);
	void LogResult(const char *operation, int status);
	void Info(const AnsiString &text);
	void SendCommand(unsigned int cmd, bool app, unsigned long arg, SdMmc::SdResponse type);
	void ReadCidCsd(void);
	static bool ParseNumber(const AnsiString &text, int base, unsigned long &value);
public:		// User declarations
	__fastcall TfrmCH341SdCard(TComponent* Owner);
	__fastcall ~TfrmCH341SdCard(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341SdCard *frmCH341SdCard;
//---------------------------------------------------------------------------
#endif
