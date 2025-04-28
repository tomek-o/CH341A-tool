//---------------------------------------------------------------------------

#ifndef FormCH341_I2C_S35390AH
#define FormCH341_I2C_S35390AH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341_I2C_S35390A : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnSetCurrentTime;
	TLabel *lblDateTime;
	TLabel *lblInt;
	TLabel *lblInt1;
	TLabel *lblInt2;
	TComboBox *cbInt1Frequency;
	TComboBox *cbInt2Frequency;
	TButton *btnSetOutputsConfig;
	TComboBox *cbInt1Sel;
	TComboBox *cbInt2Sel;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnSetCurrentTimeClick(TObject *Sender);
	void __fastcall cbInt1SelChange(TObject *Sender);
	void __fastcall cbInt2SelChange(TObject *Sender);
	void __fastcall btnSetOutputsConfigClick(TObject *Sender);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341_I2C_S35390A(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341_I2C_S35390A *frmCH341_I2C_S35390A;
//---------------------------------------------------------------------------
#endif
