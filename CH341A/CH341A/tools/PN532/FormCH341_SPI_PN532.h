//---------------------------------------------------------------------------

#ifndef FormCH341_SPI_PN532H
#define FormCH341_SPI_PN532H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "PN532.h"

//---------------------------------------------------------------------------
class TfrmCH341_SPI_PN532 : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblReadingState;
	TButton *btnStart;
	TButton *btnStop;
	TMemo *memo;
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
private:	// User declarations
	PN532 pn532;
	AnsiString version;
	bool reading;
	bool busy;
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341_SPI_PN532(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341_SPI_PN532 *frmCH341_SPI_PN532;
//---------------------------------------------------------------------------
#endif
