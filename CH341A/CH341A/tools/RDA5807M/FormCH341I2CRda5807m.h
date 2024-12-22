//---------------------------------------------------------------------------

#ifndef FormCH341I2CRda5807mH
#define FormCH341I2CRda5807mH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CRda5807m : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TButton *btnInit;
	TLabel *lblStatus;
	TButton *btnSearchDown;
	TButton *btnSearchUp;
	TLabel *lblVolume;
	TTrackBar *trbarVolume;
	TLabel *lblRdaStatus;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnSearchUpClick(TObject *Sender);
	void __fastcall btnSearchDownClick(TObject *Sender);
	void __fastcall trbarVolumeChange(TObject *Sender);
private:	// User declarations
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CRda5807m(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CRda5807m *frmCH341I2CRda5807m;
//---------------------------------------------------------------------------
#endif
