//---------------------------------------------------------------------------

#ifndef FormCH341I2CSi5351H
#define FormCH341I2CSi5351H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CSi5351 : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblStatus;
	TGroupBox *grboxCLK0;
	TGroupBox *grboxCLK2;
	TButton *btnWrite;
	TLabel *lblFrequencyA;
	TEdit *edFrequencyA;
	TLabel *lblHzA;
	TLabel *lblDriveStrength0;
	TComboBox *cbDriveStrength0;
	TLabel *lblFrequencyB;
	TLabel *lblDriveStrength2;
	TComboBox *cbDriveStrength2;
	TEdit *edFrequencyB;
	TLabel *lblHzB;
	TLabel *lblFrequencyCorrection;
	TEdit *edCorrection;
	TLabel *Label2;
	void __fastcall btnWriteClick(TObject *Sender);
private:	// User declarations

public:		// User declarations
	__fastcall TfrmCH341I2CSi5351(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CSi5351 *frmCH341I2CSi5351;
//---------------------------------------------------------------------------
#endif
