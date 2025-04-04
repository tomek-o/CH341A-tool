//---------------------------------------------------------------------------

#ifndef FormCH341I2CCH455H
#define FormCH341I2CCH455H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CCH455 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnInit;
	TLabel *lblInfo;
	TLabel *lblIntensity;
	TComboBox *cbIntensity;
	TLabel *lblMode;
	TComboBox *cbMode;
	TCheckBox *chbSleep;
	TCheckBox *chbEnable;
	TButton *btnWriteDigits;
	TEdit *edDigit0;
	TEdit *edDigit1;
	TEdit *edDigit2;
	TEdit *edDigit3;
	TLabel *lblSegmentCodes;
	TLabel *lblDigit0;
	TLabel *lblDigit1;
	TLabel *lblDigit2;
	TLabel *lblDigit3;
	TLabel *lblBits;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnWriteDigitsClick(TObject *Sender);
	void __fastcall edDigitHexChange(TObject *Sender);
	void __fastcall chbDigitBinClick(TObject *Sender);
private:	// User declarations
	void Read(void);
	TCheckBox* chbBit[4][8];
	unsigned int digitVal[4];
	void UpdateCheckboxes(void);
public:		// User declarations
	__fastcall TfrmCH341I2CCH455(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CCH455 *frmCH341I2CCH455;
//---------------------------------------------------------------------------
#endif
