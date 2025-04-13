//---------------------------------------------------------------------------

#ifndef FormCH341I2CHd44780H
#define FormCH341I2CHd44780H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CHd44780 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnWrite;
	TLabel *lblStatus;
	TLabel *lblI2CAddress;
	TComboBox *cbI2CAddress;
	TLabel *lblBacklight;
	TButton *btnBacklightOn;
	TButton *btnBacklightOff;
	TLabel *lblLine1;
	TLabel *lblLine2;
	TEdit *edLine1;
	TEdit *edLine2;
	TLabel *lblNote;
	TLabel *lblNote2;
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall btnBacklightOnClick(TObject *Sender);
	void __fastcall btnBacklightOffClick(TObject *Sender);
private:	// User declarations
	void Write(void);
	void SetBacklight(bool state);
public:		// User declarations
	__fastcall TfrmCH341I2CHd44780(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CHd44780 *frmCH341I2CHd44780;
//---------------------------------------------------------------------------
#endif
