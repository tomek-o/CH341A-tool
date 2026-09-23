//---------------------------------------------------------------------------

#ifndef FormCH341I2CMAX30102H
#define FormCH341I2CMAX30102H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <deque>
#include <stdint.h>
//---------------------------------------------------------------------------

class TfrmPlot;

class TfrmCH341I2CMAX30102 : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmrAutoRead;
	TLabel *lblStatus;
	TLabel *lblI2CSpeed;
	TLabel *lblReadingState;
	TButton *btnStart;
	TButton *btnStop;
	TPanel *pnlPlot1;
	TPanel *pnlPlot2;
	void __fastcall tmrAutoReadTimer(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
private:	// User declarations
	TfrmPlot *frmPlot1;
	TfrmPlot *frmPlot2;
	bool reading;
	bool busy;
	/** Recently accepted heart rate readings, oldest first */
	std::deque<int> hrHistory;
	/** Consecutive readings rejected as outliers */
	int hrRejectedCount;
	/** Filters raw algorithm output; returns smoothed BPM or -1 if none yet */
	int FilterHeartRate(int32_t heartRate, bool valid, AnsiString &note);
	void Read(void);
public:		// User declarations
	__fastcall TfrmCH341I2CMAX30102(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341I2CMAX30102 *frmCH341I2CMAX30102;
//---------------------------------------------------------------------------
#endif
