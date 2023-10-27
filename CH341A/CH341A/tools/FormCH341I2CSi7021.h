//---------------------------------------------------------------------------

#ifndef FormCH341I2CSi7021H
#define FormCH341I2CSi7021H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CSi7021 : public TForm
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
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
private:	// User declarations
	void Read(void);
	void Clear(void);
public:		// User declarations
	__fastcall TfrmCH341I2CSi7021(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CSi7021 *frmCH341I2CSi7021;
//---------------------------------------------------------------------------
#endif
