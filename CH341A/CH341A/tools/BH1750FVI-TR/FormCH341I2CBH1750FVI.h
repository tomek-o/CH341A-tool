//---------------------------------------------------------------------------

#ifndef FormCH341I2CBH1750FVI
#define FormCH341I2CBH1750FVI
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "BH1750FVI.h"

class TfrmCH341I2CBH1750FVI : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnOpen;
	TEdit *edRawData;
	TLabel *lblLux;
	TEdit *edLux;
	TLabel *lblRawData;
	TLabel *lblMode;
	TComboBox *cbMode;
	TButton *btnClose;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnOpenClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
private:	// User declarations
	BH1750FVI bh1750fvi;
	void Read(bool single);
	void Clear(void);
public:		// User declarations
	__fastcall TfrmCH341I2CBH1750FVI(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CBH1750FVI *frmCH341I2CBH1750FVI;
//---------------------------------------------------------------------------
#endif
