//---------------------------------------------------------------------------

#ifndef FrameUartPanelH
#define FrameUartPanelH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "ComPort.h"
//---------------------------------------------------------------------------

typedef void __fastcall (__closure *TUartDataEvent)(const unsigned char *data, unsigned int count);

/** Serial port selection panel for tools that talk to a device over a UART (not through the CH341A):
	port combobox (list refreshed when dropped down or with the Refresh button), baud rate, Open / Close
	buttons and a status text. While the port is open the panel reads it every 100 ms and passes
	what was received to onData. The owner writes to the device with GetPort().

	Place it in a panel at the top of a tool (e.g. created at runtime like the hex editor frame).
*/
class TframeUartPanel : public TFrame
{
__published:	// IDE-managed Components
	TLabel *lblPort;
	TComboBox *cbPort;
	TButton *btnRefresh;
	TLabel *lblBaud;
	TComboBox *cbBaud;
	TButton *btnOpen;
	TButton *btnClose;
	TLabel *lblState;
	TTimer *tmrRead;
	void __fastcall btnRefreshClick(TObject *Sender);
	void __fastcall cbPortDropDown(TObject *Sender);
	void __fastcall btnOpenClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall tmrReadTimer(TObject *Sender);
private:	// User declarations
	ComPort port;

	void RefreshPortList(void);
	/** Port name typed / selected in the combobox ("COM5 - USBSER000" gives "COM5") */
	std::string GetSelectedPortName(void);
	void UpdateControls(void);
public:		// User declarations
	/** Received data, called from the read timer */
	TUartDataEvent onData;
	/** The port was opened / closed (also when it was closed because of an error) */
	TNotifyEvent onOpened;
	TNotifyEvent onClosed;

	__fastcall TframeUartPanel(TComponent* Owner);
	__fastcall ~TframeUartPanel(void);

	ComPort& GetPort(void) { return port; }
	bool IsOpened(void) const { return port.IsOpened(); }
	/** Selects the baud rate shown (default 115200); added to the list if it is not there */
	void SetBaudRate(unsigned int baud);
	unsigned int GetBaudRate(void);
	/** Closes the port, \a reason is shown as the status */
	void Close(const AnsiString &reason = "Port closed");
};
//---------------------------------------------------------------------------
extern PACKAGE TframeUartPanel *frameUartPanel;
//---------------------------------------------------------------------------
#endif
