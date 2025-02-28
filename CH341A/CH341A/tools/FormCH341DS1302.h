//---------------------------------------------------------------------------

#ifndef FormCH341DS1302H
#define FormCH341DS1302H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341DS1302 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnInit;
	TButton *btnSetTrickleCharge;
	TComboBox *cbTrickleChargeMode;
	TButton *btnSetCurrentTime;
	TLabel *lblDateTime;
	TMemo *memo;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnSetTrickleChargeClick(TObject *Sender);
	void __fastcall btnSetCurrentTimeClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341DS1302(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341DS1302 *frmCH341DS1302;
//---------------------------------------------------------------------------
#endif
