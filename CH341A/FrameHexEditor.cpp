//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameHexEditor.h"
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TframeHexEditor *frameHexEditor;
//---------------------------------------------------------------------------
__fastcall TframeHexEditor::TframeHexEditor(TComponent* Owner)
	: TFrame(Owner),
	chipImage(false)
{
	hex = new THexView(this);
	hex->Parent = this;
	hex->Align = alClient;
	hex->OnCursorChange = HexCursorChange;
	hex->OnDataChange = HexDataChange;

	UpdateSource();
	UpdateStatus();
}
//---------------------------------------------------------------------------
void TframeHexEditor::SetData(const std::vector<uint8_t> &data, const AnsiString &sourceName, bool isChipImage)
{
	chipImage = isChipImage;
	hex->SetData(data.empty() ? NULL : &data[0], data.size());
	source = sourceName;
	UpdateSource();
	UpdateStatus();
}
//---------------------------------------------------------------------------
const std::vector<uint8_t>& TframeHexEditor::GetOriginal(void) const
{
	return hex->Original();
}
//---------------------------------------------------------------------------
bool TframeHexEditor::IsChipImage(void) const
{
	return chipImage;
}
//---------------------------------------------------------------------------
void TframeHexEditor::MarkClean(bool isChipImage)
{
	hex->MarkClean();
	chipImage = isChipImage;
	UpdateSource();
	UpdateStatus();
}
//---------------------------------------------------------------------------
const std::vector<uint8_t>& TframeHexEditor::GetData(void) const
{
	return hex->Data();
}
//---------------------------------------------------------------------------
bool TframeHexEditor::HasData(void) const
{
	return hex->Size() != 0;
}
//---------------------------------------------------------------------------
bool TframeHexEditor::IsModified(void) const
{
	return hex->IsModified();
}
//---------------------------------------------------------------------------
void TframeHexEditor::UpdateSource(void)
{
	AnsiString text = (source.Length() > 0) ? source : AnsiString("no data");
	if (hex->IsModified())
		text += " *";
	statusBar->Panels->Items[3]->Text = text;
}
//---------------------------------------------------------------------------
void TframeHexEditor::UpdateStatus(void)
{
	AnsiString text;
	if (hex->Size() == 0)
	{
		statusBar->Panels->Items[0]->Text = "no data";
		statusBar->Panels->Items[1]->Text = "";
		statusBar->Panels->Items[2]->Text = "";
		return;
	}

	const size_t cursor = hex->GetCursor();
	const uint8_t value = hex->Data()[cursor];
	text.sprintf("Offset 0x%08lX (%lu)", static_cast<unsigned long>(cursor), static_cast<unsigned long>(cursor));
	statusBar->Panels->Items[0]->Text = text;

	AnsiString bits;
	for (int i=7; i>=0; i--)
		bits += ((value >> i) & 1) ? "1" : "0";
	text.sprintf("Value 0x%02X = %u = %sb", value, static_cast<unsigned int>(value), bits.c_str());
	statusBar->Panels->Items[1]->Text = text;

	size_t selStart, selCount;
	hex->GetSelection(selStart, selCount);
	text.sprintf("Size %lu, selected %lu, changed %lu", static_cast<unsigned long>(hex->Size()),
		static_cast<unsigned long>(selCount), static_cast<unsigned long>(hex->ModifiedCount()));
	statusBar->Panels->Items[2]->Text = text;
}
//---------------------------------------------------------------------------
void __fastcall TframeHexEditor::HexCursorChange(TObject *Sender)
{
	UpdateStatus();
}
//---------------------------------------------------------------------------
void __fastcall TframeHexEditor::HexDataChange(TObject *Sender)
{
	UpdateSource();
	UpdateStatus();
}
//---------------------------------------------------------------------------
void __fastcall TframeHexEditor::btnOpenClick(TObject *Sender)
{
	if (!openDialog->Execute())
		return;
	try
	{
		TFileStream *fs = new TFileStream(openDialog->FileName, fmOpenRead | fmShareDenyWrite);
		try
		{
			std::vector<uint8_t> data(static_cast<size_t>(fs->Size));
			if (data.size() > 0)
				fs->ReadBuffer(&data[0], static_cast<int>(data.size()));
			SetData(data, ExtractFileName(openDialog->FileName), false);
		}
		__finally
		{
			delete fs;
		}
	}
	catch (Exception &e)
	{
		MessageBox(this->Handle, e.Message.c_str(), "Failed to read file", MB_ICONEXCLAMATION);
	}
}
//---------------------------------------------------------------------------
void __fastcall TframeHexEditor::btnSaveClick(TObject *Sender)
{
	if (!HasData())
	{
		MessageBox(this->Handle, "No data to save", "Hex editor", MB_ICONEXCLAMATION);
		return;
	}
	if (!saveDialog->Execute())
		return;
	try
	{
		TFileStream *fs = new TFileStream(saveDialog->FileName, fmCreate);
		try
		{
			const std::vector<uint8_t> &data = hex->Data();
			fs->WriteBuffer(&data[0], static_cast<int>(data.size()));
		}
		__finally
		{
			delete fs;
		}
	}
	catch (Exception &e)
	{
		MessageBox(this->Handle, e.Message.c_str(), "Failed to write file", MB_ICONEXCLAMATION);
	}
}
//---------------------------------------------------------------------------
void TframeHexEditor::GoToEdit(void)
{
	unsigned long offset;
	if (edGoto->Text.Length() == 0 || sscanf(edGoto->Text.c_str(), "%lx", &offset) != 1)
	{
		MessageBox(this->Handle, "Enter offset as hex number", "Hex editor", MB_ICONEXCLAMATION);
		return;
	}
	hex->GoTo(offset);
	hex->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TframeHexEditor::btnGotoClick(TObject *Sender)
{
	GoToEdit();
}
//---------------------------------------------------------------------------
void __fastcall TframeHexEditor::edGotoKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_RETURN)
	{
		Key = 0;
		GoToEdit();
	}
}
//---------------------------------------------------------------------------
void __fastcall TframeHexEditor::chkReadOnlyClick(TObject *Sender)
{
	hex->SetReadOnly(chkReadOnly->Checked);
}
//---------------------------------------------------------------------------
