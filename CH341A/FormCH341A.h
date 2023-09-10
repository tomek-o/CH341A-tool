//---------------------------------------------------------------------------

#ifndef FormCH341AH
#define FormCH341AH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------

#include "CH341A.h"

class TfrmCH341A : public TForm
{
__published:	// IDE-managed Components
	TButton *btnOpen;
	TButton *btnClose;
	TButton *btnRead;
	TComboBox *cbReadBytesCount;
	TLabel *lblBytes;
	TMemo *memoRead;
	TButton *btnDetectDevices;
	TMemo *memoDetect;
	void __fastcall btnOpenClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnReadClick(TObject *Sender);
	void __fastcall btnDetectDevicesClick(TObject *Sender);
private:	// User declarations
	CH341A ch341a;
public:		// User declarations
	__fastcall TfrmCH341A(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
