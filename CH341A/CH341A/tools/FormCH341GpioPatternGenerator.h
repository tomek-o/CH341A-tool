//---------------------------------------------------------------------------

#ifndef FormCH341GpioPatternGeneratorH
#define FormCH341GpioPatternGeneratorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------

#include "common/Mutex.h"
#include <mmsystem.h>
#include <Dialogs.hpp>
#include <vector>
#include <stdint.h>

class TfrmCH341GpioPatternGenerator : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblStatus;
	TPanel *pnlTop;
	TLabel *lblStepTime;
	TLabel *lblMs;
	TLabel *lblCurrentPosition;
	TLabel *lblCurrentPositionVal;
	TLabel *lblRows;
	TEdit *edStepPeriod;
	TButton *btnStart;
	TButton *btnStop;
	TEdit *edRowsCount;
	TButton *btnSetRowsCount;
	TStringGrid *Grid;
	TTimer *tmrUpdatePosition;
	TPanel *pnlBottom;
	TButton *btnSave;
	TButton *btnLoad;
	TOpenDialog *openDialog;
	TSaveDialog *saveDialog;
	void __fastcall btnSetRowsCountClick(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall tmrUpdatePositionTimer(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall GridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall GridMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall GridKeyPress(TObject *Sender, char &Key);
	void __fastcall btnSaveClick(TObject *Sender);
	void __fastcall btnLoadClick(TObject *Sender);
private:	// User declarations
	Mutex mtxPattern;
	MMRESULT timerID;
	int currentPos;
	bool started;
	void SetBit(int pos, int bit, bool state);
	std::vector<uint32_t> pattern;
	void SetRowCount(int count);
	static void CALLBACK TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
	void Stop(void);	
public:		// User declarations
	__fastcall TfrmCH341GpioPatternGenerator(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCH341GpioPatternGenerator *frmCH341GpioPatternGenerator;
//---------------------------------------------------------------------------
#endif
