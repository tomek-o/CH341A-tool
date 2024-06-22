//---------------------------------------------------------------------------

#ifndef FormCH341I2CVl53l0xH
#define FormCH341I2CVl53l0xH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CVl53l0x : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblDistance;
	TEdit *edDistance;
	TLabel *lblMm;
	TLabel *lblInfo;
	TCheckBox *chbLongRange;
	TLabel *lblMeasurementTimingBudget;
	TComboBox *cbMeasurementTimingBudget;
	TLabel *lblUs;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CVl53l0x(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CVl53l0x *frmCH341I2CVl53l0x;
//---------------------------------------------------------------------------
#endif
