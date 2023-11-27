//---------------------------------------------------------------------------

#ifndef FormCH341I2CAs5600H
#define FormCH341I2CAs5600H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CAs5600 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblRawAngle;
	TEdit *edRawAngle;
	TLabel *lblI2CAddress;
	TLabel *lblAddressNote;
	TComboBox *cbI2CAddress;
	TLabel *lblInfo;
	TLabel *lblSensorStatus;
	TEdit *edSensorStatus;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadKeyPress(TObject *Sender, char &Key);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CAs5600(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CAs5600 *frmCH341I2CAs5600;
//---------------------------------------------------------------------------
#endif
