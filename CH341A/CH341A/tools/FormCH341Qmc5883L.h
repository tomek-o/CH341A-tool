//---------------------------------------------------------------------------

#ifndef FormCH341Qmc5883LH
#define FormCH341Qmc5883LH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341Qmc5883L : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnInit;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341Qmc5883L(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341Qmc5883L *frmCH341Qmc5883L;
//---------------------------------------------------------------------------
#endif
