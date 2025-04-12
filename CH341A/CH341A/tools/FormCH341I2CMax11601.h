//---------------------------------------------------------------------------

#ifndef FormCH341I2CMax11601H
#define FormCH341I2CMax11601H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CMax11601 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblReferenceVoltage;
	TComboBox *cbReferenceVoltage;
	TLabel *lblAin0;
	TEdit *edAin0;
	TLabel *lblAin1;
	TEdit *edAin1;
	TLabel *lblAin2;
	TEdit *edAin2;
	TLabel *lblAin3;
	TEdit *edAin3;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall cbReferenceVoltageChange(TObject *Sender);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CMax11601(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CMax11601 *frmCH341I2CMax11601;
//---------------------------------------------------------------------------
#endif
