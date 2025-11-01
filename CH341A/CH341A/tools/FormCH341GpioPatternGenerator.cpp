//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341GpioPatternGenerator.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "common/GridUtils.h"
#include "common/ScopedLock.h"
#include "Log.h"
#include <assert.h>
#include <fstream>
#include <json/json.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341GpioPatternGenerator *frmCH341GpioPatternGenerator;
//---------------------------------------------------------------------------


__fastcall TfrmCH341GpioPatternGenerator::TfrmCH341GpioPatternGenerator(TComponent* Owner)
	: TForm(Owner),
	timerID(NULL),
	currentPos(-1),
	started(false)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341GpioPatternGenerator::btnSetRowsCountClick(
      TObject *Sender)
{
	int iRowCount = StrToIntDef(edRowsCount->Text, 20);
	edRowsCount->Text = iRowCount;
	SetRowCount(iRowCount);	
}
//---------------------------------------------------------------------------

void CALLBACK TfrmCH341GpioPatternGenerator::TimeProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	TfrmCH341GpioPatternGenerator *frm = reinterpret_cast<TfrmCH341GpioPatternGenerator*>(dwUser);
	ScopedLock<Mutex> lock(frm->mtxPattern);
	if (!ch341a.IsOpened())
	{
		frm->Stop();
		return;
	}
	frm->currentPos++;
	if (frm->currentPos >= static_cast<int>(frm->pattern.size()))
	{
		frm->currentPos = 0;
	}
	uint32_t val = frm->pattern[frm->currentPos];

	int status = ch341a.SetGpioOutputs(0xFFFFFFFF, val);
	if (status != 0)
	{
		frm->Stop();
		MessageBox(frm->Handle, "Error writing GPIO direction/values", frm->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}
}

void __fastcall TfrmCH341GpioPatternGenerator::btnStartClick(TObject *Sender)
{
	started = true;
	timeBeginPeriod(1);
	currentPos = -1;
	if (timerID != NULL)
	{
		timeKillEvent(timerID);
	}
	int newPeriod = StrToIntDef(edStepPeriod->Text, 500);
	edStepPeriod->Text = newPeriod;
	timerID = timeSetEvent(newPeriod, 0, TimeProc, reinterpret_cast<DWORD>(this), TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
	tmrUpdatePosition->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341GpioPatternGenerator::btnStopClick(TObject *Sender)
{
	Stop();
}

void TfrmCH341GpioPatternGenerator::Stop(void)
{
	if (!started)
		return;
	started = false;
	tmrUpdatePosition->Enabled = false;
	if (timerID != NULL)
	{
		timeKillEvent(timerID);
		timerID = NULL;
	}
	currentPos = -1;
	timeEndPeriod(1);		
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341GpioPatternGenerator::tmrUpdatePositionTimer(
      TObject *Sender)
{
	lblCurrentPositionVal->Caption = currentPos;	
}
//---------------------------------------------------------------------------

void TfrmCH341GpioPatternGenerator::SetBit(int pos, int bit, bool state)
{
	ScopedLock<Mutex> lock(mtxPattern);
	if (state)
	{
		pattern[pos] |= (1U << bit);
	}
	else
	{
		pattern[pos] &= (~1U << bit);
	}
}

void __fastcall TfrmCH341GpioPatternGenerator::FormCreate(TObject *Sender)
{
	Grid->ColWidths[0] = 65;
	int iRowCount = StrToIntDef(edRowsCount->Text, 20);
	edRowsCount->Text = iRowCount;
	SetRowCount(iRowCount);
}
//---------------------------------------------------------------------------

void TfrmCH341GpioPatternGenerator::SetRowCount(int count)
{
	Grid->RowCount = count + Grid->FixedRows;
	pattern.clear();

	Grid->Cells[0][0] = "      GPIO bit";

	// X-labels
	for (int i=Grid->FixedCols; i<Grid->ColCount; i++)
	{
		Grid->Cells[i][0] = IntToStr(Grid->ColCount - (i + Grid->FixedCols));
	}
	// Y-labels
	for (int i=Grid->FixedRows; i<Grid->RowCount; i++)
	{
		unsigned char byte = 0;
		Grid->Cells[0][i] = "Step " + IntToStr(i - Grid->FixedRows);
		pattern.push_back(byte);
	}
	for (int i=Grid->FixedCols; i<Grid->ColCount; i++)
	{
		for (int j=Grid->FixedRows; j<Grid->RowCount; j++)
		{
			Grid->Cells[i][j] = "";
			SetCheckBox(*Grid, i, j, true, false);
		}
	}
}
void __fastcall TfrmCH341GpioPatternGenerator::GridDrawCell(TObject *Sender,
      int ACol, int ARow, TRect &Rect, TGridDrawState State)
{
	TStringGrid* StringGrid = static_cast<TStringGrid*>(Sender);
	assert(StringGrid != NULL);

	TCanvas* SGCanvas = StringGrid->Canvas;
	SGCanvas->Font = StringGrid->Font;

	RECT RText = static_cast<RECT>(Rect);
	const AnsiString text(StringGrid->Cells[ACol][ARow]);

	const bool fixed = State.Contains(gdFixed);
	const bool focused = State.Contains(gdFocused);
	bool selected = State.Contains(gdSelected);
    if (!StringGrid->Options.Contains(goDrawFocusSelected))
    {
		selected = selected && !focused;
		(void)selected;
    }
    // if the cell is fixed (headers)
    if (fixed)
    {
		SGCanvas->Brush->Color = StringGrid->FixedColor;
		SGCanvas->Font->Color = clBtnText;
		SGCanvas->FillRect(Rect);
		Frame3D(SGCanvas, Rect, clBtnHighlight, clBtnShadow, 1);
	}
	/*
	  // if the cell is selected
	  else if (selected) {
		SGCanvas->Brush->Color = clHighlight;
		SGCanvas->Font->Color = clHighlightText;
		SGCanvas->FillRect(Rect);
	  }
	*/
	// if the cell is normal
	else
	{
		SGCanvas->Brush->Color = StringGrid->Color;
		SGCanvas->Font->Color = StringGrid->Font->Color;
		SGCanvas->FillRect(Rect);
	}
	// if the cell is focused
	if (focused)
	{
		DrawFocusRect(SGCanvas->Handle, &RText);
	}

	// if this cell contains a checkbox
	if (GetCheckBox(*StringGrid, ACol, ARow))
	{
		// set the flags for rendering checked/unchecked
		unsigned int state = DFCS_BUTTONCHECK;
		if (GetCheckState(*StringGrid, ACol, ARow))
		{
			state = state | DFCS_CHECKED;
			SetBit(ARow - StringGrid->FixedRows, StringGrid->ColCount - ACol - StringGrid->FixedCols, true);
		}
		else
		{
			SetBit(ARow - StringGrid->FixedRows, StringGrid->ColCount - ACol - StringGrid->FixedCols, false);
		}

		// size the checkbox
		RECT RCell =static_cast<RECT>(Rect);
		OffsetRect(&RCell, 2, (int)(0.5 * (RCell.bottom - RCell.top)));
		RCell.right = RCell.left + GetSystemMetrics(SM_CXMENUCHECK);
		RCell.bottom = RCell.top + GetSystemMetrics(SM_CYMENUCHECK);
		RCell.top -= (int)(0.5 * (RCell.bottom - RCell.top)) + 2;

		// draw the checkbox
		DrawFrameControl(StringGrid->Canvas->Handle, &RCell, DFC_BUTTON, state);

		// move the text over
		RText.left = RCell.right;
	}

	// draw the text
	RText.left += 2;
	RText.top += 2;
	DrawText(SGCanvas->Handle, text.c_str(), text.Length(), &RText, DT_LEFT |DT_VCENTER |DT_SINGLELINE);	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341GpioPatternGenerator::GridMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button != mbLeft)
	{
    	return;
	}

	TStringGrid* StringGrid = static_cast<TStringGrid*>(Sender);
	assert(StringGrid != NULL);

	int Col, Row;
	if (PtInCheckBox(*StringGrid, X, Y, Col, Row))
	{
		if (GetCheckBox(*StringGrid, Col, Row))
		{
			bool is_checked = GetCheckState(*StringGrid, Col, Row);
			SetCheckState(*StringGrid, Col, Row, !is_checked);
		}
	}	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341GpioPatternGenerator::GridKeyPress(TObject *Sender,
      char &Key)
{
	TStringGrid* StringGrid = static_cast<TStringGrid*>(Sender);
    assert(StringGrid != NULL);

    if (Key == VK_SPACE)
    {
        const int col = StringGrid->Col;
        const int row = StringGrid->Row;

		if (GetCheckBox(*StringGrid, col, row))
		{
			SetCheckState(*StringGrid, col, row, !GetCheckState(*StringGrid, col, row));
		}
	}	
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341GpioPatternGenerator::btnSaveClick(TObject *Sender)
{
	if (!saveDialog->Execute())
		return;
	AnsiString fileName = saveDialog->FileName;

	Json::Value root;
	Json::StyledWriter writer;

	int period = StrToIntDef(edStepPeriod->Text, 500);
	root["period"] = period;

	Json::Value &jPattern = root["pattern"];
	jPattern = Json::Value(Json::arrayValue);
	for (unsigned int i=0; i<pattern.size(); i++)
	{
    	jPattern.append(pattern[i]);
	}

	std::string outputConfig = writer.write( root );

	try
	{
		std::ofstream ofs(fileName.c_str());
		ofs << outputConfig;
		ofs.close();
	}
	catch(...)
	{
		MessageBox(Handle, "Failed to save file", Caption.c_str(), MB_ICONEXCLAMATION);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341GpioPatternGenerator::btnLoadClick(TObject *Sender)
{
	if (!openDialog->Execute())
		return;
	AnsiString fileName = openDialog->FileName;

	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

	Stop();

	try
	{
		std::ifstream ifs(fileName.c_str());
		std::string strConfig((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		bool parsingSuccessful = reader.parse( strConfig, root );
		if ( !parsingSuccessful )
		{
			MessageBox(Handle, "Failed to parse file as JSON", Caption.c_str(), MB_ICONEXCLAMATION);
			return;
		}
	}
	catch(...)
	{
		MessageBox(Handle, "Failed to read file", Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	int period = StrToIntDef(edStepPeriod->Text, 500);
	root.getInt("period", period);

	{
		const Json::Value &jSteps = root["pattern"];
		if (jSteps.type() == Json::arrayValue)
		{
			SetRowCount(jSteps.size());
			for (unsigned int i=0; i<jSteps.size(); i++)
			{
				pattern[i] = jSteps.get(i, 0).asUInt();
				for (unsigned int j=0; j<32; j++)
				{
					if (pattern[i] & (1U << j))
					{
						SetCheckState(*Grid, 31-j+1, i+1, true);
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------

