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
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblI2CSpeed;
	TLabel *lblReadingState;
	TButton *btnStart;
	TButton *btnStop;
	TPanel *pnlPlot1;
	TPanel *pnlPlot2;
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
private:	// User declarations
	TfrmPlot *frmPlot1;
	TfrmPlot *frmPlot2;
	bool reading;
	bool busy;
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CMAX30102(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CMAX30102 *frmCH341I2CMAX30102;
//---------------------------------------------------------------------------
#endif
