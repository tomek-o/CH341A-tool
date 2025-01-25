//---------------------------------------------------------------------------

#ifndef FormCH341MdioMmdWriteReadH
#define FormCH341MdioMmdWriteReadH
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

class TfrmCH341MdioMmdWriteRead : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblStatus;
	TButton *btnInit;
	TButton *btnReadRegister;
	TComboBox *cbPhyAddress;
	TLabel *lblPhyAddress;
	TLabel *lblInfo;
	TLabel *lblHex;
	TLabel *lblDec;
	TLabel *lblBin;
	TEdit *edValueHex;
	TEdit *edValueDec;
	TCheckListBox *chlbox;
	TButton *btnWriteRegister;
	TLabel *lblDevice;
	TLabel *lblRegister;
	TEdit *edDevice;
	TEdit *edRegister;
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall btnReadRegisterClick(TObject *Sender);
	void __fastcall edValueHexChange(TObject *Sender);
	void __fastcall edValueDecChange(TObject *Sender);
	void __fastcall chlboxClickCheck(TObject *Sender);
	void __fastcall btnWriteRegisterClick(TObject *Sender);
private:	// User declarations
	void UpdateValueDisplay(uint16_t value, TObject *Sender);
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341MdioMmdWriteRead(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341MdioMmdWriteRead *frmCH341MdioMmdWriteRead;
//---------------------------------------------------------------------------
#endif
