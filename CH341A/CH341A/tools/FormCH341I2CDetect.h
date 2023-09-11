//---------------------------------------------------------------------------

#ifndef FormCH341I2CDetectH
#define FormCH341I2CDetectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CDetect : public TForm
{
__published:	// IDE-managed Components
	TButton *btnDetectDevices;
	TMemo *memoDetect;
	void __fastcall btnDetectDevicesClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmCH341I2CDetect(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CDetect *frmCH341I2CDetect;
//---------------------------------------------------------------------------
#endif
