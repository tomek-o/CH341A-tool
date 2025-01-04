//---------------------------------------------------------------------------

#ifndef FormPlotH
#define FormPlotH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------

#include <vector>

class TfrmPlot : public TForm
{
__published:	// IDE-managed Components
	TPaintBox *paintBox;
	void __fastcall paintBoxPaint(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmPlot(TComponent* Owner);
	void DrawPlot(void);

	struct Trace
	{
		std::vector<int> samples;
		TColor color;
		Trace(void):
			color(clBlack)
		{}
	};
	std::vector<Trace> traces;
	AnsiString title;
};
//---------------------------------------------------------------------------
#endif
