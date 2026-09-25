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
#include <vector>

//---------------------------------------------------------------------------
class TfrmCH341_SPI_PN532 : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblReadingState;
	TButton *btnStart;
	TButton *btnStop;
	TButton *btnReadNdef;
	TMemo *memo;
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall btnReadNdefClick(TObject *Sender);
private:	// User declarations
	PN532 pn532;
	AnsiString version;
	bool reading;
	bool busy;
	void Read(void);
	// Read NTAG/Ultralight pages (from page 4) until at least `need` bytes are
	// collected or `capBytes`/a read error stops it. Returns true if `need` met.
	bool ReadNtagBytes(std::vector<unsigned char> &data, unsigned int need, unsigned int capBytes);
	// Read and decode the NDEF message of an activated NTAG/Ultralight tag.
	bool ReadNdef(AnsiString &out);
public:		// User declarations
	__fastcall TfrmCH341_SPI_PN532(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341_SPI_PN532 *frmCH341_SPI_PN532;
//---------------------------------------------------------------------------
#endif
