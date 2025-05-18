//---------------------------------------------------------------------------

#ifndef FormCH341SpiMax7219Led8x8CascadeH
#define FormCH341SpiMax7219Led8x8CascadeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#include <vector>

class TfrmCH341SpiMax7219Led8x8Cascade : public TForm
{
__published:	// IDE-managed Components
	TButton *btnInit;
	TLabel *lblStatus;
	TLabel *lblNote;
	TTrackBar *trbarIntensity;
	TLabel *lblIntensity;
	TShape *shapeBackground1;
	TButton *btnWrite;
	TButton *btnClear;
	TLabel *lblClickInfo;
	TShape *shapeBackground2;
	TShape *shapeBackground3;
	TShape *shapeBackground4;
	TLabel *lblCascadeCount;
	TComboBox *cbCascadeCount;
	void __fastcall btnInitClick(TObject *Sender);
	void __fastcall trbarIntensityChange(TObject *Sender);
	void __fastcall ShapeMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall btnClearClick(TObject *Sender);
	void __fastcall cbCascadeCountChange(TObject *Sender);
private:	// User declarations
	std::vector<TShape*> backgrounds;
	std::vector<TShape*> shapes;
	void Init(void);
	void Write(void);
	unsigned int GetCount(void);
public:		// User declarations
	__fastcall TfrmCH341SpiMax7219Led8x8Cascade(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341SpiMax7219Led8x8Cascade *frmCH341SpiMax7219Led8x8Cascade;
//---------------------------------------------------------------------------
#endif
