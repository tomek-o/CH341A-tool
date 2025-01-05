//---------------------------------------------------------------------------

#ifndef FormCH341I2CMFRC522H
#define FormCH341I2CMFRC522H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CMFRC522 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TButton *btnInit;
	TMemo *memo;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
private:	// User declarations
	void Read(void);
	uint8_t GetAddress(void);
	double dpsPerBit;
	AnsiString RawValToString(int val);
	void ShowReaderDetails(void);
public:		// User declarations
	__fastcall TfrmCH341I2CMFRC522(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CMFRC522 *frmCH341I2CMFRC522;
//---------------------------------------------------------------------------
#endif
