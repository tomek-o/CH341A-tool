//---------------------------------------------------------------------------

#ifndef FormMPR121H
#define FormMPR121H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "QuickMpr121.h"

class TfrmMPR121 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TButton *btnInit;
	TCheckBox *chbProximityChannel;
	TLabel *lblTouchThreshold;
	TComboBox *cbTouchThreshold;
	TLabel *lblReleaseThreshold;
	TComboBox *cbReleaseThreshold;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	mpr121 mpr121Inst;
	bool proximityEnable;
	void Read(void);
	uint8_t GetAddress(void);
public:		// User declarations
	__fastcall TfrmMPR121(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMPR121 *frmMPR121;
//---------------------------------------------------------------------------
#endif
