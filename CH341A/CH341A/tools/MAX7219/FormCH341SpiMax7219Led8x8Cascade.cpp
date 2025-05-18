/** \brief Reading temperature from LM75 I2C sensor
*/

#include <vcl.h>
#pragma hdrstop

#include "FormCH341SpiMax7219Led8x8Cascade.h"
#include "MAX7219/MAX7219.h"
#include "CH341A.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
#include <assert.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341SpiMax7219Led8x8Cascade *frmCH341SpiMax7219Led8x8Cascade;
//---------------------------------------------------------------------------

namespace
{

enum { DIGIT_COUNT = 8 };

const TColor ACTIVE_COLOR = clYellow;
const TColor INACTIVE_COLOR = 0x303030;

}	// namespace


__fastcall TfrmCH341SpiMax7219Led8x8Cascade::TfrmCH341SpiMax7219Led8x8Cascade(TComponent* Owner)
	: TForm(Owner)
{
	backgrounds.push_back(shapeBackground1);
	backgrounds.push_back(shapeBackground2);
	backgrounds.push_back(shapeBackground3);
	backgrounds.push_back(shapeBackground4);

	for (unsigned int backgroundId = 0; backgroundId < backgrounds.size(); backgroundId++)
	{
        TShape *shapeBackground = backgrounds[backgroundId];
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
				shape->Tag = (backgroundId * 8 * 8) + (row * 8) + col;
				shape->Visible = true;
				shapes.push_back(shape);
			}
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
			shapes[56], shapes[57], shapes[58], shapes[61], shapes[62], shapes[63],

			shapes[64+0], shapes[64+1], shapes[64+2], shapes[64+3], shapes[64+4], shapes[64+5], shapes[64+6],
			shapes[64+9], shapes[64+15],
			shapes[64+17], shapes[64+23],
			shapes[64+25], shapes[64+26], shapes[64+27], shapes[64+28], shapes[64+29], shapes[64+30],
			shapes[64+33], shapes[64+39],
			shapes[64+41], shapes[64+47],
			shapes[64+49], shapes[64+55],
			shapes[64+56], shapes[64+56], shapes[64+57], shapes[64+58], shapes[64+59], shapes[64+60], shapes[64+61], shapes[64+62], 

			shapes[128+2], shapes[128+3], shapes[128+4], shapes[128+5], shapes[128+6],
			shapes[128+9], shapes[128+15],
			shapes[128+16],
			shapes[128+24],
			shapes[128+32],
			shapes[128+40],
			shapes[128+49], shapes[128+55],
			shapes[128+58], shapes[128+59], shapes[128+60], shapes[128+61], shapes[128+62],

			shapes[192+0], shapes[192+1], shapes[192+2], shapes[192+3], shapes[192+4], shapes[192+5],
			shapes[192+9], shapes[192+14],
			shapes[192+17], shapes[192+23],
			shapes[192+25], shapes[192+31],
			shapes[192+33], shapes[192+39],
			shapes[192+41], shapes[192+47],
			shapes[192+49], shapes[192+54],
			shapes[192+56], shapes[192+56], shapes[192+57], shapes[192+58], shapes[192+59], shapes[192+60], shapes[192+61],

		};
		for (unsigned int i=0; i<sizeof(activeShapes)/sizeof(activeShapes[0]); i++)
		{
			activeShapes[i]->Brush->Color = ACTIVE_COLOR;
		}
	}

	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341SpiMax7219Led8x8Cascade::btnInitClick(TObject *Sender)
{
	Init();
	Write();
}

void TfrmCH341SpiMax7219Led8x8Cascade::Init(void)
{
	BtnController btnCtrl(btnInit);

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	MAX7219::initCascade(static_cast<uint8_t>(trbarIntensity->Position), DIGIT_COUNT, GetCount());
}

void TfrmCH341SpiMax7219Led8x8Cascade::Write(void)
{
	BtnController btnCtrl(btnWrite);

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	for (unsigned int id = 0; id < 8; id++)
	{
		std::vector<uint8_t> digits;
		digits.resize(GetCount());
		for (unsigned int i=0; i<digits.size(); i++)
		{
			int digitBufPos = digits.size() - 1 - i;
			digits[digitBufPos] = 0;
			for (unsigned int offset = 0; offset < 8; offset++)
			{
				unsigned int shapeId = (i * 8 * 8) + ((7-id) * 8) + offset;
				bool state = (shapes[shapeId]->Brush->Color == ACTIVE_COLOR);
				if (state)
				{
					digits[digitBufPos] |= static_cast<uint8_t>(1u << offset);
				}
			}
		}
		MAX7219::setDigitCascade(id, &digits[0], GetCount());
	}
}

//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiMax7219Led8x8Cascade::trbarIntensityChange(TObject *Sender)
{
	MAX7219::setIntensityCascade(static_cast<uint8_t>(trbarIntensity->Position), GetCount());	
}
//---------------------------------------------------------------------------


void __fastcall TfrmCH341SpiMax7219Led8x8Cascade::ShapeMouseDown(TObject *Sender,
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

void __fastcall TfrmCH341SpiMax7219Led8x8Cascade::btnWriteClick(TObject *Sender)
{
	Write();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341SpiMax7219Led8x8Cascade::btnClearClick(TObject *Sender)
{
	for (unsigned int i=0; i<shapes.size(); i++)
	{
		shapes[i]->Brush->Color = INACTIVE_COLOR;
	}
	Write();
}
//---------------------------------------------------------------------------

unsigned int TfrmCH341SpiMax7219Led8x8Cascade::GetCount(void)
{
	return cbCascadeCount->ItemIndex + 1;
}


void __fastcall TfrmCH341SpiMax7219Led8x8Cascade::cbCascadeCountChange(
      TObject *Sender)
{
	unsigned int cascadeCount = GetCount();
	for (unsigned int i=0; i<backgrounds.size(); i++)
	{
		backgrounds[i]->Visible = (i < cascadeCount);
	}
	for (unsigned int i=0; i<shapes.size(); i++)
	{
    	shapes[i]->Visible = (i < (cascadeCount * 8 * 8));
	}
}
//---------------------------------------------------------------------------

