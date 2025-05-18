//---------------------------------------------------------------------------

#ifndef FormCH341SpiST7567H
#define FormCH341SpiST7567H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341SpiST7567 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnWrite;
	TLabel *lblStatus;
	TButton *btnInit;
	TMemo *memo;
	TMemo *memoText;
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
private:	// User declarations
	void Write(void);
public:		// User declarations
	__fastcall TfrmCH341SpiST7567(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341SpiST7567 *frmCH341SpiST7567;
//---------------------------------------------------------------------------
#endif
