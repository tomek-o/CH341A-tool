//---------------------------------------------------------------------------

#ifndef FormCH341I2CCH1115H
#define FormCH341I2CCH1115H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

class CH1115;

class TfrmCH341I2CCH1115 : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbI2CAddress;
	TButton *btnInit;
	TLabel *lblContrast;
	TButton *btnInvertDisplay;
	TButton *btnTurnOn;
	TButton *btnTurnOff;
	TTrackBar *trbarContrast;
	TButton *btnClearDisplay;
	TLabel *lblInfo;
	TComboBox *cbImage;
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnInvertDisplayClick(TObject *Sender);
	void __fastcall btnTurnOnClick(TObject *Sender);
	void __fastcall btnTurnOffClick(TObject *Sender);
	void __fastcall trbarContrastChange(TObject *Sender);
	void __fastcall btnClearDisplayClick(TObject *Sender);
private:	// User declarations
	CH1115 *display;
	uint8_t GetAddress(void);
public:		// User declarations
	__fastcall TfrmCH341I2CCH1115(TComponent* Owner);
	__fastcall ~TfrmCH341I2CCH1115(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CCH1115 *frmCH341I2CCH1115;
//---------------------------------------------------------------------------
#endif
