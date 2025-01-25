//---------------------------------------------------------------------------

#ifndef FormCH341MdioDetectH
#define FormCH341MdioDetectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmCH341MdioDetect : public TForm
{
__published:	// IDE-managed Components
	TButton *btnDetectDevices;
	TMemo *memoDetect;
	TLabel *lblInfo;
	TLabel *lblStatus;
	void __fastcall btnDetectDevicesClick(TObject *Sender);
private:	// User declarations
	void Detect(void);
public:		// User declarations
	__fastcall TfrmCH341MdioDetect(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341MdioDetect *frmCH341MdioDetect;
//---------------------------------------------------------------------------
#endif
