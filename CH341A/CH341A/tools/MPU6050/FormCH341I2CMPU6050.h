//---------------------------------------------------------------------------

#ifndef FormCH341I2CMPU6050H
#define FormCH341I2CMPU6050H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CMPU6050 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TButton *btnInit;
	TLabel *lblAcceleration;
	TLabel *lblGyro;
	TEdit *edAccelerationX;
	TEdit *edAccelerationY;
	TEdit *edAccelerationZ;
	TEdit *edGyroX;
	TEdit *edGyroY;
	TEdit *edGyroZ;
	TLabel *lblRange;
	TLabel *lblDps;
	TLabel *lblAccelRange;
	TComboBox *cbGyroRange;
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
	__fastcall TfrmCH341I2CMPU6050(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CMPU6050 *frmCH341I2CMPU6050;
//---------------------------------------------------------------------------
#endif
