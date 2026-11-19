//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HexView.h"
#include <Clipbrd.hpp>
#include <string.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace
{
	const int PAD = 4;	// pixels left of the first column

	int HexDigitValue(char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		if (c >= 'a' && c <= 'f')
			return c - 'a' + 10;
		if (c >= 'A' && c <= 'F')
			return c - 'A' + 10;
		return -1;
	}

	char PrintableChar(uint8_t b)
	{
		return (b >= 32 && b < 127) ? static_cast<char>(b) : '.';
	}
}

//---------------------------------------------------------------------------
__fastcall THexView::THexView(TComponent *Owner)
	: TCustomControl(Owner),
	OnCursorChange(NULL),
	OnDataChange(NULL),
	modifiedCount(0),
	cursor(0),
	anchor(0),
	asciiPane(false),
	highNibble(true),
	readOnly(false),
	mouseSelecting(false),
	topRow(0),
	charW(8),
	lineH(16),
	headerH(18),
	measure(NULL)
{
	measure = new Graphics::TBitmap;
	Width = 480;
	Height = 240;
	TabStop = true;
	DoubleBuffered = true;
	Color = clWindow;
	Font->Name = "Courier New";
	Font->Size = 10;
	Cursor = crIBeam;
}
//---------------------------------------------------------------------------
__fastcall THexView::~THexView(void)
{
	delete measure;
}
//---------------------------------------------------------------------------
void THexView::UpdateMetrics(void)
{
	if (measure == NULL)
		return;
	measure->Canvas->Font->Assign(Font);
	charW = measure->Canvas->TextWidth("0");
	if (charW < 1)
		charW = 1;
	lineH = measure->Canvas->TextHeight("0") + 2;
	headerH = lineH + 2;
}
//---------------------------------------------------------------------------
size_t THexView::RowCount(void) const
{
	return (data.size() + BYTES_PER_ROW - 1) / BYTES_PER_ROW;
}
//---------------------------------------------------------------------------
int THexView::VisibleRows(void)
{
	UpdateMetrics();
	int rows = (ClientHeight - headerH) / lineH;
	return (rows < 1) ? 1 : rows;
}
//---------------------------------------------------------------------------
void THexView::UpdateScrollBar(void)
{
	const int visible = VisibleRows();
	const size_t rows = RowCount();
	if (rows > static_cast<size_t>(visible))
	{
		const size_t maxTop = rows - visible;
		if (topRow > maxTop)
			topRow = maxTop;
	}
	else
	{
		topRow = 0;
	}

	if (!HandleAllocated())
		return;
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
	si.nMin = 0;
	si.nMax = (rows > 0) ? static_cast<int>(rows - 1) : 0;
	si.nPage = visible;
	si.nPos = static_cast<int>(topRow);
	SetScrollInfo(Handle, SB_VERT, &si, TRUE);
}
//---------------------------------------------------------------------------
void THexView::ScrollTo(long long row)
{
	const long long visible = VisibleRows();
	const long long rows = static_cast<long long>(RowCount());
	long long maxTop = rows - visible;
	if (maxTop < 0)
		maxTop = 0;
	if (row < 0)
		row = 0;
	if (row > maxTop)
		row = maxTop;
	if (static_cast<size_t>(row) != topRow)
	{
		topRow = static_cast<size_t>(row);
		UpdateScrollBar();
		Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall THexView::WMVScroll(TMessage &Message)
{
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(Handle, SB_VERT, &si);

	const long long top = static_cast<long long>(topRow);
	const long long page = VisibleRows();
	switch (LOWORD(Message.WParam))
	{
	case SB_LINEUP:
		ScrollTo(top - 1);
		break;
	case SB_LINEDOWN:
		ScrollTo(top + 1);
		break;
	case SB_PAGEUP:
		ScrollTo(top - page);
		break;
	case SB_PAGEDOWN:
		ScrollTo(top + page);
		break;
	case SB_TOP:
		ScrollTo(0);
		break;
	case SB_BOTTOM:
		ScrollTo(static_cast<long long>(RowCount()));
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		ScrollTo(si.nTrackPos);
		break;
	default:
		break;
	}
	if (CanFocus() && !Focused())
		SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall THexView::CreateWnd(void)
{
	TCustomControl::CreateWnd();
	UpdateScrollBar();
}
//---------------------------------------------------------------------------
void __fastcall THexView::CreateParams(TCreateParams &Params)
{
	TCustomControl::CreateParams(Params);
	Params.Style |= WS_VSCROLL;
}
//---------------------------------------------------------------------------
void THexView::EnsureCursorVisible(void)
{
	const size_t row = cursor / BYTES_PER_ROW;
	const size_t visible = static_cast<size_t>(VisibleRows());
	if (row < topRow)
		topRow = row;
	else if (row >= topRow + visible)
		topRow = row - visible + 1;
	UpdateScrollBar();
}
//---------------------------------------------------------------------------
void __fastcall THexView::Resize(void)
{
	TCustomControl::Resize();
	UpdateScrollBar();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall THexView::WMGetDlgCode(TMessage &Message)
{
	Message.Result = DLGC_WANTARROWS | DLGC_WANTCHARS | DLGC_WANTTAB;
}
//---------------------------------------------------------------------------
void __fastcall THexView::WMEraseBkgnd(TMessage &Message)
{
	Message.Result = 1;
}
//---------------------------------------------------------------------------
void THexView::FireCursorChange(void)
{
	if (OnCursorChange)
		OnCursorChange(this);
}
//---------------------------------------------------------------------------
void THexView::FireDataChange(void)
{
	if (OnDataChange)
		OnDataChange(this);
}
//---------------------------------------------------------------------------
void THexView::SetData(const uint8_t *bytes, size_t count)
{
	data.assign(bytes, bytes + count);
	original = data;
	modifiedCount = 0;
	cursor = anchor = 0;
	topRow = 0;
	highNibble = true;
	UpdateScrollBar();
	Invalidate();
	FireCursorChange();
}
//---------------------------------------------------------------------------
void THexView::Resize(size_t newSize, uint8_t fill)
{
	const size_t oldSize = data.size();
	data.resize(newSize, fill);
	original.resize(newSize, fill);
	if (newSize < oldSize)
	{
		modifiedCount = 0;
		for (size_t i=0; i<newSize; i++)
		{
			if (data[i] != original[i])
				modifiedCount++;
		}
	}
	if (newSize == 0)
		cursor = anchor = 0;
	else if (cursor >= newSize)
		cursor = anchor = newSize - 1;
	UpdateScrollBar();
	Invalidate();
	FireCursorChange();
}
//---------------------------------------------------------------------------
void THexView::MarkClean(void)
{
	original = data;
	modifiedCount = 0;
	Invalidate();
}
//---------------------------------------------------------------------------
void THexView::SetReadOnly(bool state)
{
	readOnly = state;
}
//---------------------------------------------------------------------------
void THexView::GoTo(size_t offset)
{
	if (data.empty())
		return;
	MoveCursor(static_cast<long long>(offset), false);
}
//---------------------------------------------------------------------------
void THexView::GetSelection(size_t &start, size_t &count) const
{
	if (data.empty())
	{
		start = 0;
		count = 0;
		return;
	}
	const size_t a = (anchor < cursor) ? anchor : cursor;
	const size_t b = (anchor < cursor) ? cursor : anchor;
	start = a;
	count = b - a + 1;
}
//---------------------------------------------------------------------------
void THexView::SelectAll(void)
{
	if (data.empty())
		return;
	anchor = 0;
	cursor = data.size() - 1;
	Invalidate();
	FireCursorChange();
}
//---------------------------------------------------------------------------
void THexView::FillSelection(uint8_t value)
{
	if (readOnly)
		return;
	size_t start, count;
	GetSelection(start, count);
	for (size_t i=0; i<count; i++)
		SetByte(start + i, value);
	Invalidate();
	FireDataChange();
}
//---------------------------------------------------------------------------
void THexView::SetByte(size_t index, uint8_t value)
{
	if (index >= data.size())
		return;
	const bool wasModified = (data[index] != original[index]);
	data[index] = value;
	const bool isModified = (data[index] != original[index]);
	if (isModified && !wasModified)
		modifiedCount++;
	else if (!isModified && wasModified)
		modifiedCount--;
}
//---------------------------------------------------------------------------
void THexView::MoveCursor(long long newPos, bool extendSelection)
{
	if (data.empty())
		return;
	if (newPos < 0)
		newPos = 0;
	if (newPos >= static_cast<long long>(data.size()))
		newPos = static_cast<long long>(data.size()) - 1;
	cursor = static_cast<size_t>(newPos);
	if (!extendSelection)
		anchor = cursor;
	highNibble = true;
	EnsureCursorVisible();
	Invalidate();
	FireCursorChange();
}
//---------------------------------------------------------------------------
bool THexView::HitTest(int x, int y, size_t &index, bool &ascii)
{
	UpdateMetrics();
	if (y < headerH)
		return false;
	const size_t row = topRow + static_cast<size_t>((y - headerH) / lineH);
	const int c = (x - PAD) / charW;
	int b;
	if (x < PAD)
		return false;
	if (c >= ASCII_COL && c < ASCII_COL + BYTES_PER_ROW)
	{
		ascii = true;
		b = c - ASCII_COL;
	}
	else if (c >= HEX_COL)
	{
		const int rel = c - HEX_COL;
		if (rel > 48)
			return false;
		ascii = false;
		if (rel < 24)
			b = rel / 3;
		else if (rel == 24)
			b = 8;
		else
			b = (rel - 1) / 3;
		if (b > 15)
			b = 15;
	}
	else
	{
		return false;
	}
	index = row * BYTES_PER_ROW + b;
	if (index >= data.size())
		index = data.empty() ? 0 : data.size() - 1;
	return !data.empty();
}
//---------------------------------------------------------------------------
void __fastcall THexView::MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
	TCustomControl::MouseDown(Button, Shift, X, Y);
	if (CanFocus())
		SetFocus();
	if (Button != mbLeft)
		return;
	size_t index;
	bool ascii;
	if (HitTest(X, Y, index, ascii))
	{
		asciiPane = ascii;
		MoveCursor(static_cast<long long>(index), Shift.Contains(ssShift));
		mouseSelecting = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall THexView::MouseMove(Classes::TShiftState Shift, int X, int Y)
{
	TCustomControl::MouseMove(Shift, X, Y);
	if (!mouseSelecting)
		return;
	// above/below the control: select towards that end
	if (Y < headerH)
	{
		MoveCursor(static_cast<long long>(cursor) - BYTES_PER_ROW, true);
		return;
	}
	if (Y > ClientHeight)
	{
		MoveCursor(static_cast<long long>(cursor) + BYTES_PER_ROW, true);
		return;
	}
	size_t index;
	bool ascii;
	if (HitTest(X, Y, index, ascii) && index != cursor)
		MoveCursor(static_cast<long long>(index), true);
}
//---------------------------------------------------------------------------
void __fastcall THexView::MouseUp(TMouseButton Button, Classes::TShiftState Shift, int X, int Y)
{
	TCustomControl::MouseUp(Button, Shift, X, Y);
	mouseSelecting = false;
}
//---------------------------------------------------------------------------
bool __fastcall THexView::DoMouseWheelDown(Classes::TShiftState Shift, const Types::TPoint &MousePos)
{
	ScrollTo(static_cast<long long>(topRow) + 3);
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall THexView::DoMouseWheelUp(Classes::TShiftState Shift, const Types::TPoint &MousePos)
{
	ScrollTo(static_cast<long long>(topRow) - 3);
	return true;
}
//---------------------------------------------------------------------------
void __fastcall THexView::KeyDown(Word &Key, Classes::TShiftState Shift)
{
	TCustomControl::KeyDown(Key, Shift);
	if (data.empty())
		return;

	const bool shift = Shift.Contains(ssShift);
	const bool ctrl = Shift.Contains(ssCtrl);
	const long long pos = static_cast<long long>(cursor);
	const int page = VisibleRows() * BYTES_PER_ROW;

	switch (Key)
	{
	case VK_LEFT:
		MoveCursor(pos - 1, shift);
		break;
	case VK_RIGHT:
		MoveCursor(pos + 1, shift);
		break;
	case VK_UP:
		MoveCursor(pos - BYTES_PER_ROW, shift);
		break;
	case VK_DOWN:
		MoveCursor(pos + BYTES_PER_ROW, shift);
		break;
	case VK_PRIOR:
		MoveCursor(pos - page, shift);
		break;
	case VK_NEXT:
		MoveCursor(pos + page, shift);
		break;
	case VK_HOME:
		MoveCursor(ctrl ? 0 : (pos / BYTES_PER_ROW) * BYTES_PER_ROW, shift);
		break;
	case VK_END:
		MoveCursor(ctrl ? static_cast<long long>(data.size()) - 1 : (pos / BYTES_PER_ROW) * BYTES_PER_ROW + BYTES_PER_ROW - 1, shift);
		break;
	case VK_TAB:
		asciiPane = !asciiPane;
		highNibble = true;
		Invalidate();
		break;
	case 'A':
		if (ctrl)
			SelectAll();
		break;
	case 'C':
		if (ctrl)
			CopySelection();
		break;
	case 'V':
		if (ctrl)
			PasteClipboard();
		break;
	default:
		return;
	}
	Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall THexView::KeyPress(char &Key)
{
	TCustomControl::KeyPress(Key);
	if (readOnly || data.empty())
		return;

	if (asciiPane)
	{
		if (static_cast<unsigned char>(Key) >= 32 && static_cast<unsigned char>(Key) < 127)
		{
			SetByte(cursor, static_cast<uint8_t>(Key));
			FireDataChange();
			MoveCursor(static_cast<long long>(cursor) + 1, false);
			Key = 0;
		}
		return;
	}

	const int d = HexDigitValue(Key);
	if (d < 0)
		return;
	uint8_t value = data[cursor];
	if (highNibble)
	{
		value = static_cast<uint8_t>((d << 4) | (value & 0x0F));
		SetByte(cursor, value);
		highNibble = false;
		Invalidate();
		FireDataChange();
	}
	else
	{
		value = static_cast<uint8_t>((value & 0xF0) | d);
		SetByte(cursor, value);
		FireDataChange();
		if (cursor + 1 < data.size())
			MoveCursor(static_cast<long long>(cursor) + 1, false);
		else
		{
			highNibble = true;
			Invalidate();
		}
	}
	Key = 0;
}
//---------------------------------------------------------------------------
void THexView::CopySelection(void)
{
	size_t start, count;
	GetSelection(start, count);
	if (count == 0)
		return;
	AnsiString text;
	for (size_t i=0; i<count; i++)
	{
		if (asciiPane)
			text += AnsiString(PrintableChar(data[start + i]));
		else
			text.cat_printf(i == 0 ? "%02X" : " %02X", data[start + i]);
	}
	Clipboard()->AsText = text;
}
//---------------------------------------------------------------------------
void THexView::PasteClipboard(void)
{
	if (readOnly || data.empty() || !Clipboard()->HasFormat(CF_TEXT))
		return;
	const AnsiString text = Clipboard()->AsText;

	// hex pane: hex digits (any separators); ASCII pane: raw characters
	std::vector<uint8_t> bytes;
	if (asciiPane)
	{
		for (int i=1; i<=text.Length(); i++)
			bytes.push_back(static_cast<uint8_t>(text[i]));
	}
	else
	{
		int pending = -1;
		for (int i=1; i<=text.Length(); i++)
		{
			if (text[i] == '0' && i < text.Length() && (text[i + 1] == 'x' || text[i + 1] == 'X'))
			{
				i++;	// "0x" prefix, not a digit
				pending = -1;
				continue;
			}
			const int d = HexDigitValue(text[i]);
			if (d < 0)
			{
				pending = -1;	// separator ends a single digit number: "A" -> 0x0A
				continue;
			}
			if (pending < 0)
			{
				pending = d;
				const int next = (i < text.Length()) ? HexDigitValue(text[i + 1]) : -1;
				if (next < 0)
				{
					bytes.push_back(static_cast<uint8_t>(d));
					pending = -1;
				}
			}
			else
			{
				bytes.push_back(static_cast<uint8_t>((pending << 4) | d));
				pending = -1;
			}
		}
	}
	if (bytes.empty())
		return;
	size_t n = bytes.size();
	if (cursor + n > data.size())
		n = data.size() - cursor;
	for (size_t i=0; i<n; i++)
		SetByte(cursor + i, bytes[i]);
	anchor = cursor;
	cursor += n - 1;
	EnsureCursorVisible();
	Invalidate();
	FireDataChange();
	FireCursorChange();
}
//---------------------------------------------------------------------------
void __fastcall THexView::Paint(void)
{
	UpdateMetrics();
	TCanvas *c = Canvas;
	c->Font->Assign(Font);
	const int w = ClientWidth;
	c->Brush->Color = clWindow;
	c->Brush->Style = bsSolid;
	c->FillRect(TRect(0, 0, w, ClientHeight));

	// header
	c->Font->Color = clGrayText;
	c->TextOut(PAD + ADDR_COL * charW, 1, "Offset");
	for (int b=0; b<BYTES_PER_ROW; b++)
	{
		AnsiString s;
		s.sprintf("%02X", b);
		c->TextOut(PAD + HexCol(b) * charW, 1, s);
	}
	c->TextOut(PAD + ASCII_COL * charW, 1, "ASCII");
	c->Pen->Color = clBtnShadow;
	c->MoveTo(0, headerH - 1);
	c->LineTo(w, headerH - 1);

	size_t selStart = 0, selCount = 0;
	GetSelection(selStart, selCount);
	const size_t selEnd = selStart + selCount;	// exclusive

	const int rows = VisibleRows();
	for (int r=0; r<rows; r++)
	{
		const size_t row = topRow + r;
		const size_t first = row * BYTES_PER_ROW;
		if (first >= data.size())
			break;
		const int y = headerH + r * lineH;

		AnsiString addr;
		addr.sprintf("%08lX", static_cast<unsigned long>(first));
		c->Brush->Style = bsClear;
		c->Font->Color = clGrayText;
		c->TextOut(PAD + ADDR_COL * charW, y + 1, addr);

		for (int b=0; b<BYTES_PER_ROW; b++)
		{
			const size_t idx = first + b;
			if (idx >= data.size())
				break;
			const bool selected = (idx >= selStart && idx < selEnd);
			const bool modified = (data[idx] != original[idx]);
			const bool isCursor = (idx == cursor);

			AnsiString hex;
			hex.sprintf("%02X", data[idx]);
			const int hx = PAD + HexCol(b) * charW;
			const int ax = PAD + (ASCII_COL + b) * charW;

			TColor fg = modified ? clRed : clWindowText;
			if (selected)
			{
				fg = clHighlightText;
				c->Brush->Style = bsSolid;
				c->Brush->Color = clHighlight;
				// extend over the space between two selected bytes
				const bool nextSelected = (b < BYTES_PER_ROW - 1) && (idx + 1 < selEnd);
				c->FillRect(TRect(hx, y, hx + (nextSelected ? 3 : 2) * charW, y + lineH));
				c->FillRect(TRect(ax, y, ax + charW, y + lineH));
			}
			c->Brush->Style = bsClear;
			c->Font->Color = fg;
			c->TextOut(hx, y + 1, hex);
			c->TextOut(ax, y + 1, AnsiString(PrintableChar(data[idx])));

			if (isCursor)
			{
				c->Brush->Style = bsClear;
				c->Pen->Width = 1;
				// active pane: solid frame, other pane: gray frame
				c->Pen->Color = (Focused() && !asciiPane) ? clWindowText : clGrayText;
				c->Rectangle(hx - 1, y, hx + 2 * charW + 1, y + lineH);
				c->Pen->Color = (Focused() && asciiPane) ? clWindowText : clGrayText;
				c->Rectangle(ax - 1, y, ax + charW + 1, y + lineH);
				if (!asciiPane && !readOnly)
				{
					// current nibble
					const int nx = hx + (highNibble ? 0 : charW);
					c->Pen->Color = clBlue;
					c->MoveTo(nx, y + lineH - 1);
					c->LineTo(nx + charW, y + lineH - 1);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
