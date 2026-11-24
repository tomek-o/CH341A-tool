//---------------------------------------------------------------------------

#ifndef FormAT58MP1T1RS32AH
#define FormAT58MP1T1RS32AH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Graphics.hpp>

#include "FrameUartPanel.h"
#include "AT58MP1T1RS32A.h"
#include "CsvWriter.h"
#include <deque>

//---------------------------------------------------------------------------
/** AT58MP1T1RS32A 5.8 GHz motion sensor on a UART (not through the CH341A): detection state plot and
	module configuration. The serial port is selected in the UART panel at the top of the window.
*/
class TfrmAT58MP1T1RS32A : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlTop;
	TPanel *pnlUart;
	TLabel *lblDescription;
	TButton *btnScreenshot;
	TPanel *pnlBottom;
	TPaintBox *paintBox;
	TTimer *tmrRefresh;
	TLabel *lblCommands;
	TLabel *lblSetRange;
	TButton *btnWriteSetRange;
	TComboBox *cbSetRange;
	TLabel *lblRangeM;
	TLabel *lblSetSensitivity;
	TComboBox *cbSetSensitivity;
	TButton *btnSetSensitivity;
	TLabel *lblSetLatency;
	TComboBox *cbLatencyLH;
	TLabel *lblLatencyHL;
	TComboBox *cbLatencyHL;
	TButton *btnSetLatency;
	TButton *btnResetCfg;
	TButton *btnSensorStart;
	TCheckBox *chkCsv;
	void __fastcall btnScreenshotClick(TObject *Sender);
	void __fastcall tmrRefreshTimer(TObject *Sender);
	void __fastcall paintBoxPaint(TObject *Sender);
	void __fastcall btnWriteSetRangeClick(TObject *Sender);
	void __fastcall btnSetSensitivityClick(TObject *Sender);
	void __fastcall btnSetLatencyClick(TObject *Sender);
	void __fastcall btnResetCfgClick(TObject *Sender);
	void __fastcall btnSensorStartClick(TObject *Sender);
private:	// User declarations
	TframeUartPanel *uart;
	AT58MP1T1RS32A *at58s32;	// uses the port of the uart panel
	CsvWriter csvWriter;
	std::deque<unsigned int> values;
	bool refreshRequest;

	void __fastcall UartData(const unsigned char *data, unsigned int count);
	void __fastcall UartOpened(TObject *Sender);
	void __fastcall UartClosed(TObject *Sender);
	void UpdateControls(void);
	/** Reports a failure of a (slow) module command */
	void RunCommand(int status);
public:		// User declarations
	__fastcall TfrmAT58MP1T1RS32A(TComponent* Owner);
	__fastcall ~TfrmAT58MP1T1RS32A(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmAT58MP1T1RS32A *frmAT58MP1T1RS32A;
//---------------------------------------------------------------------------
#endif
