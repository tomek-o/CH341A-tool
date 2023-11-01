//---------------------------------------------------------------------------

#ifndef FormCH341I2CSsd1306H
#define FormCH341I2CSsd1306H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

namespace pico_ssd1306 {
	class SSD1306;
}

class TfrmCH341I2CSsd1306 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnWrite;
	TLabel *lblStatus;
	TLabel *lblI2CAddress;
	TComboBox *cbI2CAddress;
	TButton *btnInvertDisplay;
	TButton *btnInit;
	TLabel *lblAddressNote;
	TLabel *lblDisplayResolution;
	TComboBox *cbDisplayResolution;
	TEdit *edText;
	TButton *btnTurnOn;
	TButton *btnTurnOff;
	TLabel *lblContrast;
	TTrackBar *trbarContrast;
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall btnInvertDisplayClick(TObject *Sender);
	void __fastcall btnTurnOnClick(TObject *Sender);
	void __fastcall btnTurnOffClick(TObject *Sender);
	void __fastcall trbarContrastChange(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
private:	// User declarations
	pico_ssd1306::SSD1306 *display;
	void Write(void);
public:		// User declarations
	__fastcall TfrmCH341I2CSsd1306(TComponent* Owner);
	__fastcall ~TfrmCH341I2CSsd1306(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CSsd1306 *frmCH341I2CSsd1306;
//---------------------------------------------------------------------------
#endif
