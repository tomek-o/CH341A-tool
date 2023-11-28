//---------------------------------------------------------------------------

#ifndef FormCH341I2CAs5600H
#define FormCH341I2CAs5600H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CAs5600 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblRawAngle;
	TEdit *edRawAngle;
	TLabel *lblI2CAddress;
	TLabel *lblAddressNote;
	TComboBox *cbI2CAddress;
	TLabel *lblInfo;
	TLabel *lblSensorStatus;
	TEdit *edSensorStatus;
	TImage *image;
	TPaintBox *paintBox;
	TTrackBar *trbarTest;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall chbAutoReadClick(TObject *Sender);
	void __fastcall chbAutoReadKeyPress(TObject *Sender, char &Key);
	void __fastcall paintBoxPaint(TObject *Sender);
	void __fastcall trbarTestChange(TObject *Sender);
private:	// User declarations
	void Read(void);
	double angleRad;
public:		// User declarations
	__fastcall TfrmCH341I2CAs5600(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CAs5600 *frmCH341I2CAs5600;
//---------------------------------------------------------------------------
#endif
