//---------------------------------------------------------------------------

#ifndef FormCH341I2CBme280H
#define FormCH341I2CBme280H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CBme280 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblTemperature;
	TEdit *edTemperature;
	TLabel *lblPressure;
	TEdit *edPressure;
	TLabel *lblHumidity;
	TEdit *edHumidity;
	TLabel *lblI2CAddress;
	TComboBox *cbI2CAddress;
	TLabel *lblHumidityInfo;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CBme280(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CBme280 *frmCH341I2CBme280;
//---------------------------------------------------------------------------
#endif
