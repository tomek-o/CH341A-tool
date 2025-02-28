//---------------------------------------------------------------------------

#ifndef FormCH341TemplateH
#define FormCH341TemplateH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341Template : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TCheckBox *chbAutoRead;
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblAddress;
	TComboBox *cbAddress;
	TButton *btnInit;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
private:	// User declarations
	void Read(void);
	uint8_t GetAddress(void);
public:		// User declarations
	__fastcall TfrmCH341Template(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341Template *frmCH341Template;
//---------------------------------------------------------------------------
#endif
