//---------------------------------------------------------------------------

#ifndef FormCH341AH
#define FormCH341AH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------

#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

class TfrmCH341A : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlTop;
	TButton *btnClose;
	TButton *btnOpen;
	TTreeView *tvTools;
	TPageControl *pages;
	TTimer *tmrStartup;
	void __fastcall btnOpenClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall tvToolsChange(TObject *Sender, TTreeNode *Node);
	void __fastcall tmrStartupTimer(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	const int devIndex;
	int DeviceOpen(void);
	void DeviceClose(void);
public:		// User declarations
	__fastcall TfrmCH341A(TComponent* Owner);
	void DeviceReopen(void);
};
//---------------------------------------------------------------------------
#endif
