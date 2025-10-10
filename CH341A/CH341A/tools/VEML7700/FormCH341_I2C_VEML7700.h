//---------------------------------------------------------------------------

#ifndef FormCH341_I2C_VEML7700H
#define FormCH341_I2C_VEML7700H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "VEML7700.h"

class TfrmCH341I2CVeml7700 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
private:	// User declarations
	VEML7700 veml7700;
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CVeml7700(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CVeml7700 *frmCH341I2CVeml7700;
//---------------------------------------------------------------------------
#endif
