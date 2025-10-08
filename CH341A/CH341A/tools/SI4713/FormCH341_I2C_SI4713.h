//---------------------------------------------------------------------------

#ifndef FormCH341_I2C_SI4713H
#define FormCH341_I2C_SI4713H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include "Si4713.h"


class TfrmCH341I2CSi4713 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TButton *btnInit;
	TLabel *lblRadioStatus;
	TLabel *lblFrequency;
	TEdit *edFrequency;
	TLabel *lblMhz;
	TLabel *lblInfo;
	TEdit *edRdsStation;
	TEdit *edRdsBuffer;
	TLabel *lblRdsStation;
	TLabel *lblRdsBuffer;
	TEdit *edRdsProgramId;
	TLabel *lblRdsProgramId;
	TLabel *lblPreemphasis;
	TComboBox *cbPreemphasis;
	TButton *btnUpdateRdsBufferText;
	TButton *btnGetWinampRds;
	TCheckBox *chbRdsUpdateFromWinamp;
	TTimer *tmrUpdateRds;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnUpdateRdsBufferTextClick(TObject *Sender);
	void __fastcall btnGetWinampRdsClick(TObject *Sender);
	void __fastcall chbRdsUpdateFromWinampClick(TObject *Sender);
	void __fastcall chbRdsUpdateFromWinampMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall tmrUpdateRdsTimer(TObject *Sender);
private:	// User declarations
	Si4713 si4713;
	void Read(void);
	uint8_t GetAddress(void);
	void UpdateRdsFromWinamp(void);
public:		// User declarations
	__fastcall TfrmCH341I2CSi4713(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CSi4713 *frmCH341I2CSi4713;
//---------------------------------------------------------------------------
#endif
