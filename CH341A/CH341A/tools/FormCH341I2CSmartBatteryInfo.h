//---------------------------------------------------------------------------

#ifndef FormCH341I2CSmartBatteryInfoH
#define FormCH341I2CSmartBatteryInfoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CSmartBatteryInfo : public TForm
{
__published:	// IDE-managed Components
	TMemo *memoInfo;
	TPanel *pnlTop;
	TButton *btnReadInfo;
	TLabel *lblI2CAddress;
	TComboBox *cbI2CAddress;
	void __fastcall btnReadInfoClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmCH341I2CSmartBatteryInfo(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CSmartBatteryInfo *frmCH341I2CSmartBatteryInfo;
//---------------------------------------------------------------------------
#endif
