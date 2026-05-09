//---------------------------------------------------------------------------

#ifndef FormCH341I2CTSL2591H
#define FormCH341I2CTSL2591H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "TSL2591.h"

class TfrmCH341I2CTSL2591 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnInit;
	TLabel *lblCh0;
	TEdit *edChannel0;
	TEdit *edChannel1;
	TLabel *lblCh1;
	TEdit *edVisible;
	TLabel *lblLux;
	TEdit *edLux;
	TLabel *lblVisible;
	TLabel *lblGain;
	TComboBox *cbGain;
	TLabel *lblIntegrationTime;
	TComboBox *cbIntegrationTime;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	Adafruit_TSL2591 tsl2591;
	void Read(void);
	void Clear(void);
public:		// User declarations
	__fastcall TfrmCH341I2CTSL2591(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CTSL2591 *frmCH341I2CTSL2591;
//---------------------------------------------------------------------------
#endif
