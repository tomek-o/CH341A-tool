//---------------------------------------------------------------------------

#ifndef FormCH341I2CAds1115H
#define FormCH341I2CAds1115H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CAds1115 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TComboBox *cbMux;
	TLabel *lblMux;
	TComboBox *cbRange;
	TLabel *lblRange;
	TLabel *lblRate;
	TComboBox *cbRate;
	TLabel *lblNote;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
	bool ConversionReady(void);
	uint16_t GetConfig(void);
	void Read(void);
	uint8_t GetAddress(void);
	int WriteRegister(uint8_t reg, uint16_t value);
	int ReadRegister(uint8_t reg, uint16_t &value);
public:		// User declarations
	__fastcall TfrmCH341I2CAds1115(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CAds1115 *frmCH341I2CAds1115;
//---------------------------------------------------------------------------
#endif
