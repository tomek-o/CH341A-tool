//---------------------------------------------------------------------------

#ifndef FormCH341DS3231H
#define FormCH341DS3231H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341DS3231 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnSetCurrentTime;
	TLabel *lblDateTime;
	TButton *btnSetControl;
	TComboBox *cbControl;
	TLabel *lblOutputControl;
	TCheckBox *chbSqwOutput;
	TCheckBox *chbEnable32k;
	TCheckBox *chbBbsqw;
	TCheckBox *chbTemperatureConv;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnSetCurrentTimeClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnSetControlClick(TObject *Sender);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341DS3231(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341DS3231 *frmCH341DS3231;
//---------------------------------------------------------------------------
#endif
