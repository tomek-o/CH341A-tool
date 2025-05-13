//---------------------------------------------------------------------------

#ifndef FormCH341SpiNrf24L01H
#define FormCH341SpiNrf24L01H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341SpiNrf24L01 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnInit;
	TLabel *lblRfSpeed;
	TComboBox *cbRfSpeed;
	TLabel *lblRfChannel;
	TComboBox *cbRfChannel;
	TLabel *lblNote;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341SpiNrf24L01(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341SpiNrf24L01 *frmCH341SpiNrf24L01;
//---------------------------------------------------------------------------
#endif
