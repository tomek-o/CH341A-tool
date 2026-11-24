//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormDYP_A01.h"
#include "TabManager.h"
#include "ImageExport.h"
#include "Log.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmDYP_A01 *frmDYP_A01;

REGISTER_TOOL(TfrmDYP_A01, frmDYP_A01, 1u << ToolGroupSensors)

namespace
{
	enum { MAX_VALUES = 500 };	// shown in the plot
}

//---------------------------------------------------------------------------
__fastcall TfrmDYP_A01::TfrmDYP_A01(TComponent* Owner)
	: TForm(Owner),
	csvWriter("dyp_a01"),
	refreshRequest(false)
{
	uart = new TframeUartPanel(this);
	uart->Parent = pnlUart;
	uart->Align = alClient;
	uart->SetBaudRate(9600);	// default of the module
	uart->onData = UartData;
}
//---------------------------------------------------------------------------
__fastcall TfrmDYP_A01::~TfrmDYP_A01(void)
{
	uart->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmDYP_A01::UartData(const unsigned char *data, unsigned int count)
{
	if (dyp_a01.AddData(data, count) > 0)
		refreshRequest = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmDYP_A01::btnScreenshotClick(TObject *Sender)
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
void __fastcall TfrmDYP_A01::paintBoxPaint(TObject *Sender)
{
	const int width = paintBox->Width;
	const int height = paintBox->Height;

	TCanvas *canvas = paintBox->Canvas;
	canvas->Brush->Style = bsSolid;
	canvas->Brush->Color = clWhite;
	canvas->FillRect(canvas->ClipRect);

	canvas->Pen->Width = 1;

	const int valuesCount = static_cast<int>(values.size());

	unsigned int valMin = 0;
	unsigned int valMax = 0;
	if (!values.empty())
		valMin = valMax = values[0];
	for (std::deque<unsigned int>::iterator iter = values.begin(); iter != values.end(); ++iter)
	{
		if (*iter < valMin)
			valMin = *iter;
		if (*iter > valMax)
			valMax = *iter;
	}

	// vertical scale in whole 0.5 m steps with some room around the values
	int scaleMin = static_cast<int>(floor(static_cast<float>(valMin)/500) * 500);
	int scaleMax = static_cast<int>(ceil(static_cast<float>(valMax)/500) * 500);
	if (static_cast<int>(valMin) - scaleMin < 500)
		scaleMin -= 500;
	if (scaleMax - static_cast<int>(valMax) < 500)
		scaleMax += 500;
	if (scaleMin < 0)
		scaleMin = 0;
	if (scaleMin == scaleMax)
	{
		scaleMin -= 500;
		scaleMax += 500;
	}
	const int scaleDelta = scaleMax - scaleMin;
	const float ratioY = static_cast<float>(height) / static_cast<float>(scaleDelta);

	{
		// Y-axis grid
		canvas->Pen->Color = static_cast<TColor>(RGB(200, 200, 200));
		for (int Yval = scaleMin; Yval <= scaleMax; Yval += 250)
		{
			const float valueDelta = static_cast<float>(Yval - scaleMin);
			const int Y1 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
			canvas->MoveTo(0, Y1);
			canvas->LineTo(width, Y1);
		}
	}

	canvas->Brush->Color = clSilver;
	canvas->Pen->Color = clBlack;

	float barSpacing = 5;
	if (valuesCount > 0)
		barSpacing = static_cast<float>(width - 10) / valuesCount;
	if (barSpacing > 5)
		barSpacing = 5;
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
	for (int Yval = scaleMin; Yval <= scaleMax; Yval += 500)
	{
		const float valueDelta = static_cast<float>(Yval - scaleMin);
		const int Y1 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
		canvas->MoveTo(0, Y1);
		canvas->LineTo(8, Y1);
		AnsiString label;
		label.sprintf("%.2fm", static_cast<double>(Yval)/1000.0);
		int yOffset = 6;
		if (Yval == scaleMin)
			yOffset = 12;
		else if (Yval == scaleMax)
			yOffset = 0;
		canvas->TextOut(12, Y1 - yOffset, label);
	}

	// extra Y axis ticks
	for (int Yval = scaleMin; Yval <= scaleMax; Yval += 50)
	{
		const float valueDelta = static_cast<float>(Yval - scaleMin);
		const int Y1 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
		canvas->MoveTo(0, Y1);
		canvas->LineTo(4, Y1);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmDYP_A01::tmrRefreshTimer(TObject *Sender)
{
	if (refreshRequest == false)
		return;
	refreshRequest = false;

	std::vector<unsigned int> newValues;
	dyp_a01.GetValues(newValues);
	if (chkCsv->Checked)
		csvWriter.AddValues(newValues);
	for (unsigned int i=0; i<newValues.size(); i++)
		values.push_back(newValues[i]);
	while (values.size() > MAX_VALUES)
		values.pop_front();

	{
		unsigned int valMin = 0, valMax = 0, valLast = 0;
		double sum = 0;
		if (!values.empty())
			valMin = valMax = values[0];
		for (std::deque<unsigned int>::iterator iter = values.begin(); iter != values.end(); ++iter)
		{
			const unsigned int val = *iter;
			if (val < valMin)
				valMin = val;
			if (val > valMax)
				valMax = val;
			sum += val;
			valLast = val;
		}
		double valAvg = 0;
		if (!values.empty())
			valAvg = sum / static_cast<double>(values.size());
		AnsiString textMin, textMax, textAvg, textLast;
		textMin.sprintf("min: %.2fm", valMin / 1000.0);
		textMax.sprintf("max: %.2fm", valMax / 1000.0);
		textAvg.sprintf("avg: %.2fm", valAvg / 1000.0);
		textLast.sprintf("last: %.2fm", valLast / 1000.0);
		lblMin->Caption = textMin;
		lblMax->Caption = textMax;
		lblAvg->Caption = textAvg;
		lblLast->Caption = textLast;
	}

	paintBox->Invalidate();
}
//---------------------------------------------------------------------------
