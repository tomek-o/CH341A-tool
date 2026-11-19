//---------------------------------------------------------------------------
/** \file
    Hex editor control: offset column, 16 bytes per row in hex, ASCII column.

    Virtual - only visible rows are painted, so it handles buffers of tens of MB.
    Created in code (it is not installed in the IDE palette):
\code
	THexView *hex = new THexView(this);
	hex->Parent = this;
	hex->Align = alClient;
	hex->SetData(&bytes[0], bytes.size());
\endcode

    Keys: arrows, PgUp/PgDn, Home/End (Ctrl = whole data), Shift extends selection,
    Tab switches between hex and ASCII pane, hex digits / characters overwrite data,
    Ctrl+C copy (hex or ASCII, as the active pane), Ctrl+V paste hex bytes, Ctrl+A select all.
    Bytes that differ from the data given to SetData() / MarkClean() are shown in red.
*/

#ifndef HexViewH
#define HexViewH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <stdint.h>
#include <vector>

class THexView : public TCustomControl
{
public:
	enum { BYTES_PER_ROW = 16 };

	__fastcall THexView(TComponent *Owner);
	__fastcall ~THexView(void);

	/** Replaces contents (and the reference used for highlighting changes) */
	void SetData(const uint8_t *bytes, size_t count);
	/** Changes size, new bytes are set to \a fill. Existing bytes are kept (and reported as unchanged). */
	void Resize(size_t newSize, uint8_t fill);
	const std::vector<uint8_t>& Data(void) const { return data; }
	/** Contents as given to SetData() / MarkClean(), the reference for change highlighting */
	const std::vector<uint8_t>& Original(void) const { return original; }
	size_t Size(void) const { return data.size(); }

	/** Current contents become the reference for change highlighting */
	void MarkClean(void);
	bool IsModified(void) const { return modifiedCount != 0; }
	size_t ModifiedCount(void) const { return modifiedCount; }

	void SetReadOnly(bool state);
	bool IsReadOnly(void) const { return readOnly; }

	void GoTo(size_t offset);
	size_t GetCursor(void) const { return cursor; }
	/** Selected range, \a count is 0 if there is no data */
	void GetSelection(size_t &start, size_t &count) const;
	void SelectAll(void);
	/** Writes \a value to all selected bytes */
	void FillSelection(uint8_t value);

	/** Cursor or selection changed */
	TNotifyEvent OnCursorChange;
	/** Contents edited by the user (also paste/fill) */
	TNotifyEvent OnDataChange;

protected:
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift);
	DYNAMIC void __fastcall KeyPress(char &Key);
	DYNAMIC void __fastcall MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC bool __fastcall DoMouseWheelDown(Classes::TShiftState Shift, const Types::TPoint &MousePos);
	DYNAMIC bool __fastcall DoMouseWheelUp(Classes::TShiftState Shift, const Types::TPoint &MousePos);
	DYNAMIC void __fastcall Resize(void);
	virtual void __fastcall CreateParams(TCreateParams &Params);
	virtual void __fastcall CreateWnd(void);

	void __fastcall WMGetDlgCode(TMessage &Message);
	void __fastcall WMEraseBkgnd(TMessage &Message);
	void __fastcall WMVScroll(TMessage &Message);

BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER(WM_GETDLGCODE, TMessage, WMGetDlgCode)
	VCL_MESSAGE_HANDLER(WM_ERASEBKGND, TMessage, WMEraseBkgnd)
	VCL_MESSAGE_HANDLER(WM_VSCROLL, TMessage, WMVScroll)
END_MESSAGE_MAP(TCustomControl)

private:
	std::vector<uint8_t> data;
	std::vector<uint8_t> original;
	size_t modifiedCount;

	size_t cursor;
	size_t anchor;
	bool asciiPane;
	bool highNibble;
	bool readOnly;
	bool mouseSelecting;
	size_t topRow;

	// metrics, recalculated before painting and hit testing
	int charW;
	int lineH;
	int headerH;

	Graphics::TBitmap *measure;	// font metrics (the control canvas is not usable outside painting)

	void UpdateMetrics(void);
	size_t RowCount(void) const;
	int VisibleRows(void);
	void UpdateScrollBar(void);
	void EnsureCursorVisible(void);
	void ScrollTo(long long row);
	void FireCursorChange(void);
	void FireDataChange(void);

	void MoveCursor(long long newPos, bool extendSelection);
	void SetByte(size_t index, uint8_t value);
	bool HitTest(int x, int y, size_t &index, bool &ascii);
	void CopySelection(void);
	void PasteClipboard(void);

	// column of byte \a b (0..15) in characters
	static int HexCol(int b) { return HEX_COL + b * 3 + (b >= 8 ? 1 : 0); }
	enum {
		ADDR_COL = 0,
		HEX_COL = 10,
		ASCII_COL = 10 + 3 * 16 + 1 + 2,
		TOTAL_COLS = ASCII_COL + 16
	};
};

#endif
