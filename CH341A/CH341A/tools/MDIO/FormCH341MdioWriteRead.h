//---------------------------------------------------------------------------

#ifndef FormCH341MdioWriteReadH
#define FormCH341MdioWriteReadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <CheckLst.hpp>
//---------------------------------------------------------------------------

#include <stdint.h>

struct MdioRegDescription;

class TfrmCH341MdioWriteRead : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblStatus;
	TButton *btnInit;
	TButton *btnReadRegister;
	TComboBox *cbPhyAddress;
	TLabel *lblPhyAddress;
	TLabel *lblInfo;
	TLabel *lblRegister;
	TComboBox *cbRegId;
	TLabel *lblHex;
	TLabel *lblDec;
	TLabel *lblBin;
	TEdit *edValueHex;
	TEdit *edValueDec;
	TCheckListBox *chlbox;
	TButton *btnWriteRegister;
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnReadRegisterClick(TObject *Sender);
	void __fastcall cbRegIdChange(TObject *Sender);
	void __fastcall edValueHexChange(TObject *Sender);
	void __fastcall edValueDecChange(TObject *Sender);
	void __fastcall chlboxClickCheck(TObject *Sender);
	void __fastcall btnWriteRegisterClick(TObject *Sender);
private:	// User declarations
	const struct MdioRegDescription* FindRegDescription(int id);
	void UpdateValueDisplay(uint16_t value, TObject *Sender);
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341MdioWriteRead(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341MdioWriteRead *frmCH341MdioWriteRead;
//---------------------------------------------------------------------------
#endif
