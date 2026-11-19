//---------------------------------------------------------------------------

#ifndef FrameHexEditorH
#define FrameHexEditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <vector>
#include <stdint.h>
#include "HexView.h"
//---------------------------------------------------------------------------
/** Hex editor frame (toolbar with file open/save, hex view, status bar), meant to be placed in any tool:
	put data in with SetData(), the user edits it or loads a file, take it with GetData().
*/
class TframeHexEditor : public TFrame
{
__published:	// IDE-managed Components
	TPanel *pnlTop;
	TButton *btnOpen;
	TButton *btnSave;
	TLabel *lblGoto;
	TEdit *edGoto;
	TButton *btnGoto;
	TCheckBox *chkReadOnly;
	TStatusBar *statusBar;
	TOpenDialog *openDialog;
	TSaveDialog *saveDialog;
	void __fastcall btnOpenClick(TObject *Sender);
	void __fastcall btnSaveClick(TObject *Sender);
	void __fastcall btnGotoClick(TObject *Sender);
	void __fastcall edGotoKeyPress(TObject *Sender, char &Key);
	void __fastcall chkReadOnlyClick(TObject *Sender);
private:	// User declarations
	THexView *hex;
	AnsiString source;
	bool chipImage;

	void __fastcall HexCursorChange(TObject *Sender);
	void __fastcall HexDataChange(TObject *Sender);
	void UpdateSource(void);
	void UpdateStatus(void);
	void GoToEdit(void);
public:		// User declarations
	__fastcall TframeHexEditor(TComponent* Owner);

	/** Replaces the data, \a sourceName is shown in the status bar (file name, "chip" ...).
		\a isChipImage: data is what the chip contains (it was just read from it). */
	void SetData(const std::vector<uint8_t> &data, const AnsiString &sourceName, bool isChipImage = false);
	const std::vector<uint8_t>& GetData(void) const;
	/** Data as it was when set (or last marked clean): differences to GetData() are the user's edits */
	const std::vector<uint8_t>& GetOriginal(void) const;
	/** Data is a copy of the chip contents (as read, or as written) - edits can be written selectively */
	bool IsChipImage(void) const;
	/** Current data becomes the reference for edits, \a isChipImage: it is also what the chip contains now */
	void MarkClean(bool isChipImage);
	bool HasData(void) const;
	bool IsModified(void) const;
};
//---------------------------------------------------------------------------
extern PACKAGE TframeHexEditor *frameHexEditor;
//---------------------------------------------------------------------------
#endif
