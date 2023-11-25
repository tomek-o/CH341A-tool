//---------------------------------------------------------------------------

#ifndef FormCH341SpiWriteReadH
#define FormCH341SpiWriteReadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfrmCH341SpiWriteRead : public TForm
{
__published:	// IDE-managed Components
	TButton *btnWriteRead;
	TMemo *memoRead;
	TMemo *memoWrite;
	TLabel *lblWrite;
	TLabel *lblWriteBytesCount;
	TLabel *lblDataRead;
	void __fastcall btnWriteReadClick(TObject *Sender);
	void __fastcall memoWriteChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmCH341SpiWriteRead(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341SpiWriteRead *frmCH341SpiWriteRead;
//---------------------------------------------------------------------------
#endif
