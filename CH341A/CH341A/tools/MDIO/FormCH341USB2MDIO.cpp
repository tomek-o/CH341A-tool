//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341USB2MDIO.h"
#include "CH341A.h"
#include "MDIO.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
#include <stdio.h>
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341USB2MDIO *frmCH341USB2MDIO;
//---------------------------------------------------------------------------


__fastcall TfrmCH341USB2MDIO::TfrmCH341USB2MDIO(TComponent* Owner)
	: TForm(Owner)
{
	TabManager::Instance().Register(this, (1 << ToolGroupEthernetMdio));
	for (unsigned int i=0; i<32; i++)
	{
		cbPhyAddress->Items->Add(i);
	}
	cbPhyAddress->ItemIndex = 0;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341USB2MDIO::btnLoadScriptClick(TObject *Sender)
{
	if (openDialog->Execute())
		Load(openDialog->FileName);	
}
//---------------------------------------------------------------------------

void TfrmCH341USB2MDIO::Load(AnsiString fileName)
{
	lvScript->Items->Count = 0;
	lvScript->Invalidate();
	scriptLines.clear();

	std::auto_ptr<TStrings> strings(new TStringList());
	try
	{
		strings->LoadFromFile(fileName);
	}
	catch(...)
	{
		MessageBox(this->Handle, "Failed to load data from file.", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	for (int i=0; i<strings->Count; i++)
	{
		ScriptLine scriptLine;
		scriptLine.script = strings->Strings[i];
		scriptLines.push_back(scriptLine);
	}

	lvScript->Items->Count = scriptLines.size();
	lvScript->Invalidate();
}

void __fastcall TfrmCH341USB2MDIO::lvScriptData(TObject *Sender,
      TListItem *Item)
{
	int id = Item->Index;
	Item->Caption = id;
	const ScriptLine &scriptLine = scriptLines[id];
	Item->SubItems->Add(scriptLine.script);
	Item->SubItems->Add(scriptLine.result);
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341USB2MDIO::btnRunScriptClick(TObject *Sender)
{
	BtnController btnCtrl(btnRunScript);

	for (unsigned int i=0; i<scriptLines.size(); i++)
	{
		scriptLines[i].result = "";
	}
	lvScript->Invalidate();

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}
	if (MDIO_Init())
	{
		lblStatus->Caption = "Failed to configure CH341A pins!";
		return;
	}
	lblStatus->Caption = "";

	for (unsigned int i=0; i<scriptLines.size(); i++)
	{
		int status = RunLine(scriptLines[i]);
		if (status != 0)
		{
			break;
		}
	}
}
//---------------------------------------------------------------------------

int TfrmCH341USB2MDIO::RunLine(ScriptLine &scriptLine)
{
	unsigned int phyAddress = this->cbPhyAddress->ItemIndex;
	AnsiString script = scriptLine.script.Trim().UpperCase();
	uint32_t reg = 0;

	int commentPos = script.Pos("//");
	if (commentPos)
	{
		script = script.SubString(1, commentPos - 1);
		script = script.Trim();
	}

	switch (script.Length())
	{
	case 4:
		for (unsigned int i=1; i<=4; i++)
		{
			if (!isxdigit(script[i]))
			{
				scriptLine.result = "Unexpected non-hex character!";
				return -1;
			}
		}
		if (sscanf(script.c_str(), "%X", &reg) != 1)
		{
			scriptLine.result = "Unexpected hex value!";
			return -1;
		}
		else
		{
			unsigned int val = MDIO_TI_ExtendedRead(phyAddress, reg);
			scriptLine.result.sprintf("Read: 0x%04X", val);
		}
		break;
	case 9:
		script = StringReplace(script, " ", "", TReplaceFlags() << rfReplaceAll);
		if (script.Length() != 8)	// expecting "ABCD EF01": register + value to write
		{
			scriptLine.result = "Unexpected line format!";
			return -1;
		}
		// no break
	case 8:
		// "write" script line might also have no space between register and value
		for (unsigned int i=1; i<=8; i++)
		{
			if (!isxdigit(script[i]))
			{
				scriptLine.result = "Unexpected non-hex character!";
				return -1;
			}
		}
		{
			char str1[5], str2[5];
			memset(str1, 0, sizeof(str1));
			memset(str2, 0, sizeof(str2));
			memcpy(str1, script.c_str(), 4);
			memcpy(str2, script.c_str() + 4, 4);
			if (sscanf(str1, "%X", &reg) != 1)
			{
				scriptLine.result = "Unexpected hex value!";
				return -1;
			}
			unsigned int val = 0;
			if (sscanf(str2, "%X", &val) != 1)
			{
				scriptLine.result = "Unexpected hex value!";
				return -1;
			}
			MDIO_TI_ExtendedWrite(phyAddress, reg, static_cast<uint16_t>(val));
			scriptLine.result.sprintf("Write to 0x%04X: 0x%04X", reg, val);
		}
		
		break;
	}

	return 0;
}

