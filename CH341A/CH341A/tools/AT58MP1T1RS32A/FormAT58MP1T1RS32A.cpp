//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormAT58MP1T1RS32A.h"
#include "TabManager.h"
#include "ImageExport.h"
#include "Log.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmAT58MP1T1RS32A *frmAT58MP1T1RS32A;

REGISTER_TOOL(TfrmAT58MP1T1RS32A, frmAT58MP1T1RS32A, 1u << ToolGroupSensors)

namespace
{
	enum { MAX_VALUES = 200 };	// shown in the plot
}

//---------------------------------------------------------------------------
__fastcall TfrmAT58MP1T1RS32A::TfrmAT58MP1T1RS32A(TComponent* Owner)
	: TForm(Owner),
	csvWriter("at58mp1t1rs32a"),
	refreshRequest(false)
{
	uart = new TframeUartPanel(this);
	uart->Parent = pnlUart;
	uart->Align = alClient;
	uart->SetBaudRate(115200);
	uart->onData = UartData;
	uart->onOpened = UartOpened;
	uart->onClosed = UartClosed;

	at58s32 = new AT58MP1T1RS32A(uart->GetPort());
	UpdateControls();
}
//---------------------------------------------------------------------------
__fastcall TfrmAT58MP1T1RS32A::~TfrmAT58MP1T1RS32A(void)
{
	uart->Close();
	delete at58s32;
}
//---------------------------------------------------------------------------
void TfrmAT58MP1T1RS32A::UpdateControls(void)
{
	pnlBottom->Enabled = uart->IsOpened();	// module commands
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::UartData(const unsigned char *data, unsigned int count)
{
	if (at58s32->addData(data, count) > 0)
		refreshRequest = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::UartOpened(TObject *Sender)
{
	UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::UartClosed(TObject *Sender)
{
	UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::btnScreenshotClick(TObject *Sender)
{
	Graphics::TBitmap *bmp = this->GetFormImage();

	const AnsiString dir = ExtractFilePath(Application->ExeName) + "screenshots\\";
	ForceDirectories(dir);
	const AnsiString fileName = dir + FormatDateTime("yyyymmdd_hhmmss_zzz", Now()) + "_screenshot.png";
	const int status = WriteImage(fileName, bmp, IMAGE_EXPORT_FORMAT_PNG);
	if (status != 0)
		LOG("Failed to write %s (error %d)\n", fileName.c_str(), status);
	else
		LOG("Screenshot saved to %s\n", fileName.c_str());

	delete bmp;
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::tmrRefreshTimer(TObject *Sender)
{
	if (refreshRequest == false)
		return;
	refreshRequest = false;

	std::vector<unsigned int> newValues;
	at58s32->getValues(newValues);
	if (chkCsv->Checked)
		csvWriter.AddValues(newValues);
	for (unsigned int i=0; i<newValues.size(); i++)
		values.push_back(newValues[i]);
	while (values.size() > MAX_VALUES)
		values.pop_front();

	paintBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::paintBoxPaint(TObject *Sender)
{
	const int width = paintBox->Width;
	const int height = paintBox->Height;

	TCanvas *canvas = paintBox->Canvas;
	canvas->Brush->Style = bsSolid;
	canvas->Brush->Color = clWhite;
	canvas->FillRect(canvas->ClipRect);

	const int valuesCount = static_cast<int>(values.size());

	const int scaleMin = -1;
	const int scaleMax = 2;
	const int scaleDelta = scaleMax - scaleMin;
	const float ratioY = static_cast<float>(height) / static_cast<float>(scaleDelta);

	{
		// Y-axis grid
		canvas->Pen->Width = 1;
		canvas->Pen->Color = static_cast<TColor>(RGB(200, 200, 200));
		for (int Yval = 0; Yval <= 1; Yval++)
		{
			const float valueDelta = static_cast<float>(Yval - scaleMin);
			const int Y1 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
			canvas->MoveTo(0, Y1);
			canvas->LineTo(width, Y1);
		}
	}

	canvas->Brush->Color = clSilver;
	canvas->Pen->Color = clBlack;

	enum { BAR_SPACING_MAX = 10 };
	float barSpacing = BAR_SPACING_MAX;
	if (valuesCount > 0)
		barSpacing = static_cast<float>(width - 10) / valuesCount;
	if (barSpacing > BAR_SPACING_MAX)
		barSpacing = BAR_SPACING_MAX;
	float barWidth = barSpacing - 2;
	if (barWidth <= 1)
		barWidth = 1;

	{
		unsigned int pos = 0;
		for (std::deque<unsigned int>::iterator iter = values.begin(); iter != values.end(); ++iter)
		{
			const int X1 = 10 + static_cast<int>(pos*barSpacing);
			const int Y1 = height;
			const int X2 = X1 + static_cast<int>(barWidth);
			const float valueDelta = static_cast<float>(static_cast<int>(*iter) - scaleMin);
			const int Y2 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
			canvas->Rectangle(X1, Y1, X2, Y2);
			pos++;
		}
	}

	canvas->Brush->Color = clWhite;	// text background

	// Y axis scale
	for (int Yval = 0; Yval <= 1; Yval++)
	{
		const float valueDelta = static_cast<float>(Yval - scaleMin);
		const int Y1 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
		canvas->MoveTo(0, Y1);
		canvas->LineTo(8, Y1);
		AnsiString label;
		label.sprintf("%d", Yval);
		canvas->TextOut(12, Y1 - 6, label);
	}
}
//---------------------------------------------------------------------------
void TfrmAT58MP1T1RS32A::RunCommand(int status)
{
	if (status != 0)
		MessageBox(this->Handle, "Failed to write commands", this->Caption.c_str(), MB_ICONEXCLAMATION);
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::btnWriteSetRangeClick(TObject *Sender)
{
	TCursor oldCursor = Screen->Cursor;
	Screen->Cursor = crHourGlass;	// the module takes about 2 s for a command sequence
	const int status = at58s32->setRange(static_cast<unsigned int>(StrToInt(cbSetRange->Text)));
	Screen->Cursor = oldCursor;
	RunCommand(status);
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::btnSetSensitivityClick(TObject *Sender)
{
	TCursor oldCursor = Screen->Cursor;
	Screen->Cursor = crHourGlass;
	const int status = at58s32->setSensitivity(StrToInt(cbSetSensitivity->Text));
	Screen->Cursor = oldCursor;
	RunCommand(status);
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::btnSetLatencyClick(TObject *Sender)
{
	TCursor oldCursor = Screen->Cursor;
	Screen->Cursor = crHourGlass;
	const int status = at58s32->setOutputLatency(StrToInt(cbLatencyLH->Text), StrToInt(cbLatencyHL->Text));
	Screen->Cursor = oldCursor;
	RunCommand(status);
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::btnResetCfgClick(TObject *Sender)
{
	TCursor oldCursor = Screen->Cursor;
	Screen->Cursor = crHourGlass;
	const int status = at58s32->factoryReset();
	Screen->Cursor = oldCursor;
	RunCommand(status);
}
//---------------------------------------------------------------------------
void __fastcall TfrmAT58MP1T1RS32A::btnSensorStartClick(TObject *Sender)
{
	RunCommand(at58s32->sensorStart());
}
//---------------------------------------------------------------------------
