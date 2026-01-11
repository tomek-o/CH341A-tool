/** \brief Reading temperature from LM75 I2C sensor
*/

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SpiMax7219Led8x8.h"
#include "MAX7219/MAX7219.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SpiMax7219Led8x8 *frmCH341SpiMax7219Led8x8;
//---------------------------------------------------------------------------

namespace
{

enum { DIGIT_COUNT = 8 };

const TColor ACTIVE_COLOR = clYellow;
const TColor INACTIVE_COLOR = 0x303030;

}	// namespace


__fastcall TfrmCH341SpiMax7219Led8x8::TfrmCH341SpiMax7219Led8x8(TComponent* Owner)
	: TForm(Owner)
{
	for (unsigned int row = 0; row < 8; row++)
	{
		for (unsigned int col=0; col < 8; col++)
		{
			TShape *shape = new TShape(this);
			shape->Parent = this;
			shape->Shape = stCircle;
			shape->Width = shapeBackground->Width / 8;
			shape->Height = shapeBackground->Height / 8;
			shape->Left = shapeBackground->Left + (col * shapeBackground->Width / 8);
			shape->Top = shapeBackground->Top + (row * shapeBackground->Height / 8);
			shape->Brush->Color = INACTIVE_COLOR;
			shape->OnMouseDown = ShapeMouseDown;
			shape->Tag = row * 8 + col;
			shape->Visible = true;
			shapes.push_back(shape);
		}
	}

	{
		// example, default LED pattern - "A" letter
		TShape* activeShapes[] =
		{
			shapes[3], shapes[4],
			shapes[10], shapes[13],
			shapes[17], shapes[22],
			shapes[25], shapes[30],
			shapes[33], shapes[34], shapes[35], shapes[36], shapes[37], shapes[38],
			shapes[41], shapes[46],
			shapes[49], shapes[54],
			shapes[56], shapes[57], shapes[58], shapes[61], shapes[62], shapes[63]
		};
		for (unsigned int i=0; i<sizeof(activeShapes)/sizeof(activeShapes[0]); i++)
		{
			activeShapes[i]->Brush->Color = ACTIVE_COLOR;
		}
	}

	TabManager::Instance().Register(this, 1u << ToolGroupDisplays);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SpiMax7219Led8x8::btnInitClick(TObject *Sender)
{
	Init();
	Write();
}

void TfrmCH341SpiMax7219Led8x8::Init(void)
{
	BtnController btnCtrl(btnInit);

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	MAX7219::init(static_cast<uint8_t>(trbarIntensity->Position), DIGIT_COUNT);
}

void TfrmCH341SpiMax7219Led8x8::Write(void)
{
	BtnController btnCtrl(btnWrite);

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t digits[MAX7219::DIGITS];
	assert(sizeof(digits)*8 == shapes.size());
	memset(digits, 0, sizeof(digits));

	for (unsigned int i=0; i<shapes.size(); i++)
	{
		bool state = (shapes[i]->Brush->Color == ACTIVE_COLOR);
		if (state)
		{
        	digits[7 - (i/8)] |= static_cast<uint8_t>(1 << (i%8));
		}
	}

	MAX7219::setDigits(digits);	
}

//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiMax7219Led8x8::trbarIntensityChange(TObject *Sender)
{
	MAX7219::setIntensity(static_cast<uint8_t>(trbarIntensity->Position));	
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341SpiMax7219Led8x8::ShapeMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	TShape *shape = dynamic_cast<TShape*>(Sender);
	assert(shape);
	if (shape->Brush->Color == INACTIVE_COLOR)
		shape->Brush->Color = ACTIVE_COLOR;
	else
		shape->Brush->Color = INACTIVE_COLOR;
	Write();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiMax7219Led8x8::btnWriteClick(TObject *Sender)
{
	Write();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiMax7219Led8x8::btnClearClick(TObject *Sender)
{
	for (unsigned int i=0; i<shapes.size(); i++)
	{
		shapes[i]->Brush->Color = INACTIVE_COLOR;
	}
	Write();
}
//---------------------------------------------------------------------------

