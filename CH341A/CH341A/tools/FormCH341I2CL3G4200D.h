//---------------------------------------------------------------------------

#ifndef FormCH341I2CL3G4200DH
#define FormCH341I2CL3G4200DH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CL3G4200D : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TButton *btnInit;
	TLabel *lblX;
	TLabel *lblY;
	TLabel *lblZ;
	TEdit *edX;
	TEdit *edY;
	TEdit *edZ;
	TLabel *lblRange;
	TComboBox *cbRange;
	TLabel *lblDps;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
private:	// User declarations
	void Read(void);
	uint8_t GetAddress(void);
	double dpsPerBit;
	AnsiString RawValToString(int val);
public:		// User declarations
	__fastcall TfrmCH341I2CL3G4200D(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CL3G4200D *frmCH341I2CL3G4200D;
//---------------------------------------------------------------------------
#endif
