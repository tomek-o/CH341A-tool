//---------------------------------------------------------------------------

#ifndef FormCH341I2CLm75H
#define FormCH341I2CLm75H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CLm75 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblTemperature;
	TEdit *edTemperature;
	TLabel *lblI2CAddress;
	TLabel *lblAddressNote;
	TComboBox *cbI2CAddress;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CLm75(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CLm75 *frmCH341I2CLm75;
//---------------------------------------------------------------------------
#endif
