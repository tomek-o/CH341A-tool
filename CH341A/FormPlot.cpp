//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormPlot.h"
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfrmPlot::TfrmPlot(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TfrmPlot::DrawPlot(void)
{
	paintBox->Invalidate();
}

void __fastcall TfrmPlot::paintBoxPaint(TObject *Sender)
{
	int width = paintBox->Width;
	int height = paintBox->Height;

	TCanvas *canvas = paintBox->Canvas;
	canvas->Brush->Style = bsSolid;
	//
	canvas->Brush->Color = clWhite;
	canvas->FillRect(canvas->ClipRect);

	canvas->Brush->Color = clGray;
	canvas->Pen->Width = 1;

	int valMin = 0;
	int valMax = 0;
	unsigned int valuesCount = 0;
	for (unsigned int i=0; i<traces.size(); i++)
	{
		const Trace &trace = traces[i];
		if (!trace.samples.empty())
		{
			valMin = valMax = trace.samples[0];
			break;
		}
	}

	for (unsigned int i=0; i<traces.size(); i++)
	{
		const Trace &trace = traces[i];
		if (trace.samples.size() > valuesCount)
		{
			valuesCount = trace.samples.size();
		}
		for (std::vector<int>::const_iterator iter = trace.samples.begin(); iter != trace.samples.end(); ++iter)
		{
			int val = *iter;
			if (val < valMin)
				valMin = val;
			if (val > valMax)
				valMax = val;
		}
	}

	int span = valMax - valMin;
	int margin = (span / 400) * 100;
	if (margin == 0)
		margin = 1;
	int scaleMin = static_cast<int>(floor(static_cast<float>(valMin)/margin) * margin);
	int scaleMax = static_cast<int>(ceil(static_cast<float>(valMax)/margin) * margin);
	if (valMin - scaleMin < margin)
		scaleMin -= margin;
	if (scaleMax - valMax < margin)
		scaleMax += margin;
	if (scaleMin < 0)
		scaleMin = 0;
	if (scaleMin == scaleMax)
	{
		scaleMin -= 100;
		scaleMax += 100;
	}
	int scaleDelta = scaleMax - scaleMin;

	float ratioY = static_cast<float>(height) / static_cast<float>(scaleDelta);

	{
		// Y-axis grid
		canvas->Pen->Color = static_cast<TColor>(RGB(200, 200, 200));
		//canvas->Brush->Style = bsDiagCross;
		int delta = (scaleMax - scaleMin) / 20;
		if (delta > 0)
		{
			for (int Yval = scaleMin; Yval <= scaleMax; Yval += delta)
			{
				int X1 = 0;
				float valueDelta = Yval - scaleMin;
				int Y1 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
				canvas->MoveTo(X1 + 0, Y1);
				canvas->LineTo(X1 + width, Y1);
			}
		}
	}


	canvas->Brush->Color = clSilver;
	canvas->Pen->Color = clBlack;

#if 0
	float barSpacing = 5;
	if (valuesCount > 0)
		barSpacing = static_cast<float>(width - 10) / valuesCount;
	if (barSpacing > 5)
		barSpacing = 5;
	float barWidth = barSpacing - 2;
	if (barWidth <= 1)
		barWidth = 1;

	for (unsigned int i=0; i<traces.size(); i++)
	{
		const Trace &trace = traces[i];

		canvas->Pen->Color = trace.color;

		unsigned int pos = 0;
		for (std::vector<int>::const_iterator iter = trace.samples.begin(); iter != trace.samples.end(); ++iter)
		{
			int X1 = 10 + static_cast<int>(pos*barSpacing);
			int Y1 = height;
			int X2 = X1 + static_cast<int>(barWidth);
			float valueDelta = *iter - scaleMin;
			int Y2 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
			canvas->Rectangle(X1, Y1, X2, Y2);
			pos++;
		}
	}
#else
	float spacing = 5;
	if (valuesCount > 0)
		spacing = static_cast<float>(width - 10) / valuesCount;
	for (unsigned int i=0; i<traces.size(); i++)
	{
		const Trace &trace = traces[i];

		canvas->Pen->Color = trace.color;

		unsigned int pos = 0;
		for (std::vector<int>::const_iterator iter = trace.samples.begin(); iter != trace.samples.end(); ++iter)
		{
			int X1 = 10 + static_cast<int>(pos*spacing);
			float valueDelta = *iter - scaleMin;
			int Y1 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
			if (pos == 0)
				canvas->MoveTo(X1, Y1);
			else
				canvas->LineTo(X1, Y1);
			pos++;
		}
	}
#endif

	canvas->Brush->Color = clWhite;	// text background
	canvas->Pen->Color = clBlack;

	// Y axis scale
	unsigned int delta = (scaleMax - scaleMin) / 10;
	if (delta > 0)
	{
		for (int Yval = scaleMin; Yval <= scaleMax; Yval += delta)
		{
			int X1 = 0;
			float valueDelta = Yval - scaleMin;
			int Y1 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
			canvas->MoveTo(X1, Y1);
			canvas->LineTo(X1 + 8, Y1);
			AnsiString asLabel;
			asLabel.sprintf("%d", Yval);
			int yOffset = 6;
			if (Yval == scaleMin)
				yOffset = 12;
			else if (Yval == scaleMax)
				yOffset = 0;
			canvas->TextOutA(X1 + 12, Y1 - yOffset, asLabel);
		}

		if (delta / 2 > 0)
		{
			// extra Y axis ticks
			for (int Yval = scaleMin; Yval <= scaleMax; Yval += delta/2)
			{
				int X1 = 0;
				float valueDelta = Yval - scaleMin;
				int Y1 = height - static_cast<int>(floor((valueDelta * ratioY) + 0.5f));
				canvas->MoveTo(X1, Y1);
				canvas->LineTo(X1 + 4, Y1);
			}
		}
	}
	canvas->TextOutA(80, 10, title);
}
//---------------------------------------------------------------------------
