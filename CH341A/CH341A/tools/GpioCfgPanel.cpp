//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "GpioCfgPanel.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TGpioCfgPanel *)
{
	static id = 0;
	new TGpioCfgPanel(NULL, id, "test");
}
//---------------------------------------------------------------------------
__fastcall TGpioCfgPanel::TGpioCfgPanel(TComponent* Owner, unsigned int id, AnsiString description)
	: TPanel(Owner),
	id(id)
{
	Width = 390;
	Height = 22;

	int left = 16;
	int top = 2;

	AnsiString text;
	if (description != "")
	{
		text.sprintf("D%u (%s)", id, description.c_str());
	}
	else
	{
		text.sprintf("D%u", id);
	}

	label = new TLabel(this);
	label->Caption = text;
	label->Parent = this;
	label->Left = left;
	label->Top = top;
	label->Visible = true;

	left += 100;
	chbDirection = new TCheckBox(this);
	chbDirection->Parent = this;
	chbDirection->Left = left;
	chbDirection->Top = top;
	chbDirection->Visible = true;

	left += 100;
	chbDataOut = new TCheckBox(this);
	chbDataOut->Parent = this;
	chbDataOut->Left = left;
	chbDataOut->Top = top;
	chbDataOut->Visible = true;

	left += 100;
	lblDataIn = new TLabel(this);
	lblDataIn->Caption = "?";
	lblDataIn->Parent = this;
	lblDataIn->Left = left;
	lblDataIn->Top = top;
	lblDataIn->Visible = true;
}


//---------------------------------------------------------------------------
namespace Gpiocfgpanel
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TGpioCfgPanel)};
		RegisterComponents("Samples", classes, 0);
	}
}
//---------------------------------------------------------------------------
