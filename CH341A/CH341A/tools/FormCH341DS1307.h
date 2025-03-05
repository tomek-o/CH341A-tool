//---------------------------------------------------------------------------

#ifndef FormCH341DS1307H
#define FormCH341DS1307H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341DS1307 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnSetCurrentTime;
	TLabel *lblDateTime;
	TButton *btnSetTrickleCharge;
	TComboBox *cbControl;
	TLabel *lblNote;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnSetCurrentTimeClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnSetTrickleChargeClick(TObject *Sender);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341DS1307(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341DS1307 *frmCH341DS1307;
//---------------------------------------------------------------------------
#endif
