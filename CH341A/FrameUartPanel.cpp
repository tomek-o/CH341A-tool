//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameUartPanel.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TframeUartPanel *frameUartPanel;
//---------------------------------------------------------------------------
__fastcall TframeUartPanel::TframeUartPanel(TComponent* Owner)
	: TFrame(Owner),
	onData(NULL),
	onOpened(NULL),
	onClosed(NULL)
{
	cbBaud->Items->Add("1200");
	cbBaud->Items->Add("2400");
	cbBaud->Items->Add("4800");
	cbBaud->Items->Add("9600");
	cbBaud->Items->Add("19200");
	cbBaud->Items->Add("38400");
	cbBaud->Items->Add("57600");
	cbBaud->Items->Add("115200");
	cbBaud->Items->Add("230400");
	SetBaudRate(115200);

	RefreshPortList();
	UpdateControls();
}
//---------------------------------------------------------------------------
__fastcall TframeUartPanel::~TframeUartPanel(void)
{
	tmrRead->Enabled = false;
	port.Close();
}
//---------------------------------------------------------------------------
void TframeUartPanel::SetBaudRate(unsigned int baud)
{
	const AnsiString text = IntToStr(static_cast<int>(baud));
	int index = cbBaud->Items->IndexOf(text);
	if (index < 0)
		index = cbBaud->Items->Add(text);
	cbBaud->ItemIndex = index;
}
//---------------------------------------------------------------------------
unsigned int TframeUartPanel::GetBaudRate(void)
{
	return static_cast<unsigned int>(StrToIntDef(cbBaud->Text, 115200));
}
//---------------------------------------------------------------------------
void TframeUartPanel::RefreshPortList(void)
{
	const AnsiString previous = GetSelectedPortName().c_str();

	std::vector<ComPort::PortInfo> ports;
	ComPort::EnumeratePorts(ports);
	cbPort->Items->Clear();
	int select = -1;
	for (size_t i=0; i<ports.size(); i++)
	{
		AnsiString text = ports[i].name.c_str();
		if (!ports[i].description.empty())
			text += AnsiString(" - ") + ports[i].description.c_str();
		cbPort->Items->Add(text);
		if (previous == ports[i].name.c_str())
			select = static_cast<int>(i);
	}
	if (select >= 0)
		cbPort->ItemIndex = select;
	else if (previous.IsEmpty() && !ports.empty())
		cbPort->ItemIndex = 0;
	else
		cbPort->Text = previous;	// a port that is not (or no longer) listed can still be typed
}
//---------------------------------------------------------------------------
std::string TframeUartPanel::GetSelectedPortName(void)
{
	AnsiString text = cbPort->Text.Trim();
	const int space = text.Pos(" ");
	if (space > 0)
		text = text.SubString(1, space - 1);
	return text.c_str();
}
//---------------------------------------------------------------------------
void TframeUartPanel::UpdateControls(void)
{
	const bool opened = port.IsOpened();
	cbPort->Enabled = !opened;
	btnRefresh->Enabled = !opened;
	cbBaud->Enabled = !opened;
	btnOpen->Enabled = !opened;
	btnClose->Enabled = opened;
}
//---------------------------------------------------------------------------
void TframeUartPanel::Close(const AnsiString &reason)
{
	const bool wasOpened = port.IsOpened();
	tmrRead->Enabled = false;
	port.Close();
	lblState->Caption = reason;
	UpdateControls();
	if (wasOpened && onClosed)
		onClosed(this);
}
//---------------------------------------------------------------------------
void __fastcall TframeUartPanel::btnRefreshClick(TObject *Sender)
{
	RefreshPortList();
}
//---------------------------------------------------------------------------
void __fastcall TframeUartPanel::cbPortDropDown(TObject *Sender)
{
	RefreshPortList();
}
//---------------------------------------------------------------------------
void __fastcall TframeUartPanel::btnOpenClick(TObject *Sender)
{
	const std::string name = GetSelectedPortName();
	if (name.empty())
	{
		MessageBox(this->Handle, "Select the serial port first", "UART", MB_ICONEXCLAMATION);
		return;
	}
	const unsigned int baud = GetBaudRate();
	if (port.Open(name, baud) != 0)
	{
		AnsiString text;
		text.sprintf("Failed to open %s: %s", name.c_str(), port.GetLastErrorText().c_str());
		lblState->Caption = text;
		LOG("%s\n", text.c_str());
		return;
	}
	AnsiString text;
	text.sprintf("%s opened, %u baud", name.c_str(), baud);
	lblState->Caption = text;
	LOG("%s\n", text.c_str());
	tmrRead->Enabled = true;
	UpdateControls();
	if (onOpened)
		onOpened(this);
}
//---------------------------------------------------------------------------
void __fastcall TframeUartPanel::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TframeUartPanel::tmrReadTimer(TObject *Sender)
{
	if (!port.IsOpened())
		return;

	unsigned char buffer[256];
	for (int round = 0; round < 8; round++)	// drain what has been received, but do not hog the UI
	{
		const int count = port.Read(buffer, sizeof(buffer));
		if (count < 0)
		{
			AnsiString text = AnsiString("Port error: ") + port.GetLastErrorText().c_str();
			LOG("%s\n", text.c_str());
			Close(text);
			return;
		}
		if (count > 0 && onData)
			onData(buffer, static_cast<unsigned int>(count));
		if (!port.IsOpened())	// the handler closed it
			return;
		if (count < static_cast<int>(sizeof(buffer)))
			break;
	}
}
//---------------------------------------------------------------------------
