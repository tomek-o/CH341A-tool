//---------------------------------------------------------------------------

#ifndef FormCH341I2CSHT4xH
#define FormCH341I2CSHT4xH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CSHT4x : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblTemperature;
	TEdit *edTemperature;
	TLabel *lblHumidity;
	TEdit *edHumidity;
	TLabel *lblInfo;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TLabel *lblPrecision;
	TComboBox *cbPrecision;
	TButton *btnReadWithHeater;
	TComboBox *cbHeater;
	TButton *btnReadSerial;
	TEdit *edSerial;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnReadWithHeaterClick(TObject *Sender);
	void __fastcall btnReadSerialClick(TObject *Sender);
private:	// User declarations
	void Read(bool heaterOn);
	void Clear(void);
public:		// User declarations
	__fastcall TfrmCH341I2CSHT4x(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CSHT4x *frmCH341I2CSHT4x;
//---------------------------------------------------------------------------
#endif
