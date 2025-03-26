//---------------------------------------------------------------------------

#ifndef FormCH341SHT11H
#define FormCH341SHT11H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341SHT11 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TLabel *lblSupplyVoltage;
	TComboBox *cbSupplyVoltage;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341SHT11(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341SHT11 *frmCH341SHT11;
//---------------------------------------------------------------------------
#endif
