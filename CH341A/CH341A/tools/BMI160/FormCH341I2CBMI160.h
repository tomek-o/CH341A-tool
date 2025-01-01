//---------------------------------------------------------------------------

#ifndef FormCH341I2CBMI160H
#define FormCH341I2CBMI160H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CBMI160 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TButton *btnInit;
	TLabel *lblRange;
	TComboBox *cbGyroRange;
	TLabel *lblDps;
	TLabel *lblAcceleration;
	TEdit *edAccelerationX;
	TEdit *edAccelerationY;
	TEdit *edAccelerationZ;
	TLabel *lblGyro;
	TEdit *edGyroX;
	TEdit *edGyroY;
	TEdit *edGyroZ;
	TLabel *lblAccelRange;
	TComboBox *cbAccelerationRange;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadEnter(TObject *Sender);
private:	// User declarations
	void Read(void);
	uint8_t GetAddress(void);
	double dpsPerBit;
	AnsiString RawValToString(int val);
	void Clear(void);
public:		// User declarations
	__fastcall TfrmCH341I2CBMI160(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CBMI160 *frmCH341I2CBMI160;
//---------------------------------------------------------------------------
#endif
