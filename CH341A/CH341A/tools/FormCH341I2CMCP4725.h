//---------------------------------------------------------------------------

#ifndef FormCH341I2CMCP4725H
#define FormCH341I2CMCP4725H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CMCP4725 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TMemo *memo;
	TLabel *lblValue;
	TEdit *edValue;
	TButton *btnWriteDacFastMode;
	TLabel *lblVal;
	TButton *btnWriteDacAndEeprom;
	TLabel *lblRead;
	void __fastcall btnWriteDacFastModeClick(TObject *Sender);
	void __fastcall edValueChange(TObject *Sender);
	void __fastcall btnWriteDacAndEepromClick(TObject *Sender);
	void __fastcall btnReadClick(TObject *Sender);
private:	// User declarations
	void Read(void);
	uint8_t GetAddress(void);
public:		// User declarations
	__fastcall TfrmCH341I2CMCP4725(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CMCP4725 *frmCH341I2CMCP4725;
//---------------------------------------------------------------------------
#endif
