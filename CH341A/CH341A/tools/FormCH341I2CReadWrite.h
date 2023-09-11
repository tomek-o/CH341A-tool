//---------------------------------------------------------------------------

#ifndef FormCH341I2CReadWriteH
#define FormCH341I2CReadWriteH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CReadWrite : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblBytes;
	TButton *btnRead;
	TComboBox *cbReadBytesCount;
	TMemo *memoRead;
	void __fastcall btnReadClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmCH341I2CReadWrite(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CReadWrite *frmCH341I2CReadWrite;
//---------------------------------------------------------------------------
#endif
