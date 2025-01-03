//---------------------------------------------------------------------------

#ifndef FormCH341I2CMAX30102H
#define FormCH341I2CMAX30102H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

class TfrmPlot;

class TfrmCH341I2CMAX30102 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TButton *btnInit;
	TPanel *pnlPlot1;
	TPanel *pnlPlot2;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
private:	// User declarations
	TfrmPlot *frmPlot1;
	TfrmPlot *frmPlot2;
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CMAX30102(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CMAX30102 *frmCH341I2CMAX30102;
//---------------------------------------------------------------------------
#endif
