//---------------------------------------------------------------------------

#ifndef FormCH341GpioHd44780H
#define FormCH341GpioHd44780H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341GpioHd44780 : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TPanel *pnlBottom;
	TButton *btnWrite;
	TLabel *lblStatus;
	void __fastcall btnWriteClick(TObject *Sender);
private:	// User declarations
	void Write(void);
public:		// User declarations
	__fastcall TfrmCH341GpioHd44780(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341GpioHd44780 *frmCH341GpioHd44780;
//---------------------------------------------------------------------------
#endif
