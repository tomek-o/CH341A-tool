//---------------------------------------------------------------------------

#ifndef FormCH341GpioH
#define FormCH341GpioH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

class TGpioCfgPanel;

class TfrmCH341Gpio : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblWarning;
	TScrollBox *scrollBox;
	TFlowPanel *panel;
	TLabel *lblLegend;
	TPanel *pnlBottom;
	TButton *btnWrite;
	TButton *btnRead;
	TCheckBox *chbReadPeriodically;
	TTimer *timerRead;
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall timerReadTimer(TObject *Sender);
private:	// User declarations
	TGpioCfgPanel* panels[32];
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341Gpio(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341Gpio *frmCH341Gpio;
//---------------------------------------------------------------------------
#endif
