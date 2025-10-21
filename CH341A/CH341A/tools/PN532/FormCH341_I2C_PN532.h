//---------------------------------------------------------------------------

#ifndef FormCH341_I2C_PN532H
#define FormCH341_I2C_PN532H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "PN532.h"

//---------------------------------------------------------------------------
class TfrmCH341_I2C_PN532 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnInit;
	TMemo *memo;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	PN532 pn532;
	AnsiString version;
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341_I2C_PN532(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341_I2C_PN532 *frmCH341_I2C_PN532;
//---------------------------------------------------------------------------
#endif
