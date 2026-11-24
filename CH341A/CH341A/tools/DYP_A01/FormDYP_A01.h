//---------------------------------------------------------------------------

#ifndef FormDYP_A01H
#define FormDYP_A01H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Graphics.hpp>

#include "FrameUartPanel.h"
#include "DYP_A01.h"
#include "CsvWriter.h"
#include <deque>

//---------------------------------------------------------------------------
/** DYP-A01 ultrasonic distance sensor on a UART (not through the CH341A): distance plot with
	min / max / average. The serial port is selected in the UART panel at the top of the window.
*/
class TfrmDYP_A01 : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlTop;
	TPanel *pnlUart;
	TLabel *lblDescription;
	TButton *btnScreenshot;
	TPanel *pnlBottom;
	TPaintBox *paintBox;
	TLabel *lblMin;
	TLabel *lblMax;
	TLabel *lblAvg;
	TLabel *lblLast;
	TCheckBox *chkCsv;
	TTimer *tmrRefresh;
	void __fastcall btnScreenshotClick(TObject *Sender);
	void __fastcall paintBoxPaint(TObject *Sender);
	void __fastcall tmrRefreshTimer(TObject *Sender);
private:	// User declarations
	TframeUartPanel *uart;
	DYP_A01 dyp_a01;
	CsvWriter csvWriter;
	std::deque<unsigned int> values;
	bool refreshRequest;

	void __fastcall UartData(const unsigned char *data, unsigned int count);
public:		// User declarations
	__fastcall TfrmDYP_A01(TComponent* Owner);
	__fastcall ~TfrmDYP_A01(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmDYP_A01 *frmDYP_A01;
//---------------------------------------------------------------------------
#endif
