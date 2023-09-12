//---------------------------------------------------------------------------

#ifndef FormCH341I2CWriteReadH
#define FormCH341I2CWriteReadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfrmCH341I2CWriteRead : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblBytes;
	TButton *btnRead;
	TComboBox *cbReadBytesCount;
	TMemo *memoRead;
	void __fastcall btnReadClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmCH341I2CWriteRead(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CWriteRead *frmCH341I2CWriteRead;
//---------------------------------------------------------------------------
#endif
