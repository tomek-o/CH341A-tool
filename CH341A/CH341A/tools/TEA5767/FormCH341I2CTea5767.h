//---------------------------------------------------------------------------

#ifndef FormCH341I2CTea5767H
#define FormCH341I2CTea5767H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CTea5767 : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmrAutoRead;
	TButton *btnInit;
	TLabel *lblStatus;
	TButton *btnSearchDown;
	TButton *btnSearchUp;
	TLabel *lblTeaStatus;
	TLabel *lblTeaStatus2;
	TButton *btnTuneM50;
	TButton *btnTuneM10;
	TButton *btnTuneP10;
	TButton *btnTuneP50;
	TButton *btnStopHandling;
	TLabel *lblManualTuning;
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnSearchUpClick(TObject *Sender);
	void __fastcall btnSearchDownClick(TObject *Sender);
	void __fastcall btnTuneByClick(TObject *Sender);
	void __fastcall btnStopHandlingClick(TObject *Sender);
private:	// User declarations
	bool searching;
	bool searchingUp;
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CTea5767(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CTea5767 *frmCH341I2CTea5767;
//---------------------------------------------------------------------------
#endif
