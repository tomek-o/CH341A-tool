//---------------------------------------------------------------------------

#ifndef FormCH341I2CApds9960H
#define FormCH341I2CApds9960H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CApds9960 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblLight;
	TLabel *lblProximity;
	TEdit *edProximity;
	TButton *btnInit;
	TLabel *lblGesture;
	TEdit *edLight;
	TMemo *memoGestures;
	TCheckBox *chbReadGestures;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CApds9960(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CApds9960 *frmCH341I2CApds9960;
//---------------------------------------------------------------------------
#endif
