//---------------------------------------------------------------------------

#ifndef FormCH341OneWireH
#define FormCH341OneWireH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "OneWire.h"
//---------------------------------------------------------------------------
class TfrmCH341OneWire : public TForm
{
__published:	// IDE-managed Components
	TMemo *memInfo;
	TLabel *lblPin;
	TComboBox *cbPin;
	TLabel *lblReadTiming;
	TEdit *edReadLow;
	TEdit *edReadWait;
	TButton *btnCheck;
	TButton *btnReset;
	TButton *btnReadRom;
	TButton *btnSearch;
	TLabel *lblWrite;
	TEdit *edWrite;
	TLabel *lblReadCount;
	TEdit *edReadCount;
	TCheckBox *chbResetFirst;
	TButton *btnWriteRead;
	TMemo *memLog;
	TLabel *lblStatus;
	void __fastcall btnCheckClick(TObject *Sender);
	void __fastcall btnResetClick(TObject *Sender);
	void __fastcall btnReadRomClick(TObject *Sender);
	void __fastcall btnSearchClick(TObject *Sender);
	void __fastcall btnWriteReadClick(TObject *Sender);
private:	// User declarations
	CH341OneWire ow;
	/** Applies pin and timing selection, checks that CH341 is opened */
	bool Prepare(void);
	void Report(int status);
	void Log(const AnsiString &text);
public:		// User declarations
	__fastcall TfrmCH341OneWire(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341OneWire *frmCH341OneWire;
//---------------------------------------------------------------------------
#endif
