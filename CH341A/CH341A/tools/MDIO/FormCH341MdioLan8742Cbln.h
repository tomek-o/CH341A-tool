//---------------------------------------------------------------------------

#ifndef FormCH341MdioLan8742CblnH
#define FormCH341MdioLan8742CblnH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341MdioLan8742Cbln : public TForm
{
__published:	// IDE-managed Components
	TButton *btnRead;
	TLabel *lblStatus;
	TButton *btnInit;
	TLabel *lblInfo;
	TLabel *lblDesc;
	TLabel *lblCbln;
	TEdit *edCbln;
	TLabel *lblEstLen;
	TEdit *edLength;
	TLabel *lblM;
	TLabel *lblPhyAddress;
	TComboBox *cbPhyAddress;
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
private:	// User declarations
	void Read(void);
	uint8_t GetAddress(void);
public:		// User declarations
	__fastcall TfrmCH341MdioLan8742Cbln(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341MdioLan8742Cbln *frmCH341MdioLan8742Cbln;
//---------------------------------------------------------------------------
#endif
