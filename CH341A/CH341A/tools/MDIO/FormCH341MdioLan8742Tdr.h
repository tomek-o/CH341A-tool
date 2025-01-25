//---------------------------------------------------------------------------

#ifndef FormCH341MdioLan8742TdrH
#define FormCH341MdioLan8742TdrH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341MdioLan8742Tdr : public TForm
{
__published:	// IDE-managed Components
	TButton *btnTest;
	TLabel *lblStatus;
	TButton *btnInit;
	TLabel *lblInfo;
	TLabel *lblDesc;
	TLabel *lblPhyAddress;
	TComboBox *cbPhyAddress;
	TComboBox *cbPair;
	TLabel *lblCableType;
	TComboBox *cbCableType;
	TGroupBox *grboxTx;
	TLabel *lblCableTxState;
	TEdit *edCableTxState;
	TLabel *lblCableTxLengthRaw;
	TEdit *edCableTxLengthReg;
	TLabel *lblCableTxLengthMeters;
	TEdit *edCableTxLengthMeters;
	TGroupBox *grboxRx;
	TLabel *lblCableRxState;
	TLabel *lblCableRxLengthRaw;
	TLabel *lblCableRxLengthMeters;
	TEdit *edCableRxState;
	TEdit *edCableRxLengthReg;
	TEdit *edCableRxLengthMeters;
	void __fastcall btnTestClick(TObject *Sender);
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall cbCableTypeChange(TObject *Sender);
private:	// User declarations
	struct Result
	{
		unsigned int cableType;
		unsigned int cableLengthReg;
		Result(void):
			cableType(0),
			cableLengthReg(0)
		{}
	} resultTx, resultRx;
	void Read(void);
	void TestPair(unsigned int phyAddress, bool txPair, TEdit *edState, TEdit *edLengthReg, TEdit *edLengthMeters);
	void UpdateLength(void);
	float GetPropagationConst(const Result &res);
public:		// User declarations
	__fastcall TfrmCH341MdioLan8742Tdr(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341MdioLan8742Tdr *frmCH341MdioLan8742Tdr;
//---------------------------------------------------------------------------
#endif
