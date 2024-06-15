//---------------------------------------------------------------------------

#ifndef FormCH341SpiMax7219Led8x8H
#define FormCH341SpiMax7219Led8x8H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include <vector>

class TfrmCH341SpiMax7219Led8x8 : public TForm
{
__published:	// IDE-managed Components
	TButton *btnInit;
	TLabel *lblStatus;
	TLabel *lblNote;
	TTrackBar *trbarIntensity;
	TLabel *lblIntensity;
	TShape *shapeBackground;
	TButton *btnWrite;
	TButton *btnClear;
	TLabel *lblClickInfo;
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall trbarIntensityChange(TObject *Sender);
	void __fastcall ShapeMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall btnClearClick(TObject *Sender);
private:	// User declarations
	std::vector<TShape*> shapes;
	void Init(void);
	void Write(void);
public:		// User declarations
	__fastcall TfrmCH341SpiMax7219Led8x8(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341SpiMax7219Led8x8 *frmCH341SpiMax7219Led8x8;
//---------------------------------------------------------------------------
#endif
