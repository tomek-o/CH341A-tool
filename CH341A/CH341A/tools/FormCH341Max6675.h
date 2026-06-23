//---------------------------------------------------------------------------

#ifndef FormCH341Max6675H
#define FormCH341Max6675H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341Max6675 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341Max6675(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341Max6675 *frmCH341Max6675;
//---------------------------------------------------------------------------
#endif
