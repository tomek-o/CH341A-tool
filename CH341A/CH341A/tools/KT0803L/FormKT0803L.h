//---------------------------------------------------------------------------

#ifndef FormKT0803LH
#define FormKT0803LH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmKT0803L : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblStatus;
	TButton *btnInit;
	TLabel *lblFrequency;
	TLabel *lblPreemphasis;
	TComboBox *cbPreemphasis;
	TEdit *edFrequency;
	TLabel *lblMhz;
	TCheckBox *chbHighPilotAmplitude;
	void __fastcall btnInitClick(TObject *Sender);
private:	// User declarations

public:		// User declarations
	__fastcall TfrmKT0803L(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmKT0803L *frmKT0803L;
//---------------------------------------------------------------------------
#endif
