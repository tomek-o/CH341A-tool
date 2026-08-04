//---------------------------------------------------------------------------

#ifndef FormCH341_TM1637H
#define FormCH341_TM1637H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "TM1637.h"

class TfrmCH341Tm1637 : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblStatus;
	TButton *btnInit;
	TButton *btnWrite;
	TLabel *lblConnectionInfo;
	TLabel *lblInfo;
	TLabel *lblBrightness;
	TComboBox *cbBrightness;
	TEdit *edText;
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall cbBrightnessChange(TObject *Sender);
private:	// User declarations
	TM1637 tm1637;
public:		// User declarations
	__fastcall TfrmCH341Tm1637(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341Tm1637 *frmCH341Tm1637;
//---------------------------------------------------------------------------
#endif
