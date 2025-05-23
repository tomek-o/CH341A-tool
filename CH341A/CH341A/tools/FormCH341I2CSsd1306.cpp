//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CSsd1306.h"
#include "CH341A.h"
#include "ssd1306/ssd1306.h"
#include "ssd1306/textRenderer/TextRenderer.h"
#include "ssd1306/shapeRenderer/ShapeRenderer.h"
#include "TabManager.h"
#include "common/BtnController.h"
#include "Log.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CSsd1306 *frmCH341I2CSsd1306;
//---------------------------------------------------------------------------

namespace
{
	unsigned int animationCycle = 0;
}	// namespace


__fastcall TfrmCH341I2CSsd1306::TfrmCH341I2CSsd1306(TComponent* Owner)
	: TForm(Owner),
	display(NULL)
{
	TabManager::Instance().Register(this);
	for (unsigned int i=0; i<128; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbI2CAddress->Items->Add(text);
	}
	cbI2CAddress->ItemIndex = 0x3C;
}

__fastcall TfrmCH341I2CSsd1306::~TfrmCH341I2CSsd1306(void)
{
	if (display)
		delete display;
}

//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CSsd1306::btnWriteClick(TObject *Sender)
{
	Write();
}

void TfrmCH341I2CSsd1306::Write(void)
{
	BtnController btnCtrl(btnWrite);

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	if (display == NULL)
	{
		lblStatus->Caption = "Display is not initialized!";
		return;
	}	

    display->clear();

    // Draw text on display
    // After passing a pointer to display, we need to tell the function what font and text to use
    // Available fonts are listed in textRenderer's readme
    // Last we tell this function where to anchor the text
    // Anchor means top left of what we draw
	pico_ssd1306::drawText(display, font_12x16, edText->Text.c_str(), 0 ,0);

	// Send buffer to the display
	display->sendBuffer();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSsd1306::btnInvertDisplayClick(TObject *Sender)
{
	BtnController btnCtrl(btnInvertDisplay);

	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	if (display == NULL)
	{
		lblStatus->Caption = "Display is not initialized!";
		return;
	}

	display->invertDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSsd1306::btnTurnOnClick(TObject *Sender)
{
	BtnController btnCtrl(dynamic_cast<TButton*>(Sender));

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	if (display == NULL)
	{
		lblStatus->Caption = "Display is not initialized!";
		return;
	}

	display->turnOn();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSsd1306::btnTurnOffClick(TObject *Sender)
{
	BtnController btnCtrl(dynamic_cast<TButton*>(Sender));

	lblStatus->Caption = "";
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	if (display == NULL)
	{
		lblStatus->Caption = "Display is not initialized!";
		return;
	}

	display->turnOff();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSsd1306::trbarContrastChange(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	if (display == NULL)
	{
		lblStatus->Caption = "Display is not initialized!";
		return;
	}
	display->setContrast(static_cast<unsigned char>(trbarContrast->Position));
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSsd1306::btnInitClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	uint8_t address = static_cast<uint8_t>(cbI2CAddress->ItemIndex);

	int status = ch341a.I2CCheckDev(address);
	if (status != 0)
	{
		lblStatus->Caption = "No ACK after sending address!";
		return;
	}

	(void)font_5x8;
	(void)font_8x8;
	(void)font_12x16;
	(void)font_16x32;

	if (display)
	{
		delete display;
		display = NULL;
	}

	display = new pico_ssd1306::SSD1306(ch341a, address, static_cast<pico_ssd1306::Size>(cbDisplayResolution->ItemIndex));

    // Here we rotate the display by 180 degrees, so that it's not upside down from my perspective
    // If your screen is upside down try setting it to 1 or 0
    display->setOrientation(0);

    // Draw text on display
    // After passing a pointer to display, we need to tell the function what font and text to use
    // Available fonts are listed in textRenderer's readme
    // Last we tell this function where to anchor the text
    // Anchor means top left of what we draw
	pico_ssd1306::drawText(display, font_12x16, edText->Text.c_str(), 0 ,0);

	// Send buffer to the display
	display->sendBuffer();

	lblStatus->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSsd1306::btnAnimationStartClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		lblStatus->Caption = "CH341 is not opened!";
		return;
	}

	if (display == NULL)
	{
		lblStatus->Caption = "Display is not initialized!";
		return;
	}

	tmrAnimate->Enabled = true;
	btnAnimationStart->Enabled = false;
	btnAnimationStop->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSsd1306::btnAnimationStopClick(TObject *Sender)
{
	tmrAnimate->Enabled = false;
	btnAnimationStart->Enabled = true;
	btnAnimationStop->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmCH341I2CSsd1306::tmrAnimateTimer(TObject *Sender)
{
	if (!ch341a.IsOpened() || display == NULL)
	{
		return;
	}

	pico_ssd1306::Size size = static_cast<pico_ssd1306::Size>(cbDisplayResolution->ItemIndex);
	unsigned int w, h;
	if (size == pico_ssd1306::W128xH64)
	{
		w = 128;
		h = 64;
	}
	else
	{
		w = 128;
		h = 32;
	}

	display->clear();

	switch (animationCycle)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		{
			for (unsigned int i=animationCycle; i<h; i+=4)
			{
            	pico_ssd1306::drawLine(display, 0, static_cast<uint8_t>(i), static_cast<uint8_t>(w-1), static_cast<uint8_t>(i));
			}
		}
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		{
			for (unsigned int i=animationCycle-4; i<w; i+=4)
			{
            	pico_ssd1306::drawLine(display, static_cast<uint8_t>(i), 0, static_cast<uint8_t>(i), static_cast<uint8_t>(h-1));
			}
		}
		break;
	case 8:
		{
			for (unsigned int y=0; y<h; y++)
			{
				for (unsigned int x=0; x<w; x++)
				{
					if ((x % 2) == (y % 2))
					{
						display->setPixel(static_cast<uint8_t>(x), static_cast<uint8_t>(y));
					}
				}
			}
		}
		break;
	case 9:
		{
			for (unsigned int y=0; y<h; y++)
			{
				for (unsigned int x=0; x<w; x++)
				{
					if ((x % 2) != (y % 2))
					{
						display->setPixel(static_cast<uint8_t>(x), static_cast<uint8_t>(y));
					}
				}
			}
		}
		break;

	case 10:
		{
			for (unsigned int y=0; y<h; y++)
			{
				for (unsigned int x=0; x<w; x++)
				{
					if (((x/2) % 2) == ((y/2) % 2))
					{
						display->setPixel(static_cast<uint8_t>(x), static_cast<uint8_t>(y));
					}
				}
			}
		}
		break;
	case 11:
		{
			for (unsigned int y=0; y<h; y++)
			{
				for (unsigned int x=0; x<w; x++)
				{
					if (((x/2) % 2) != ((y/2) % 2))
					{
						display->setPixel(static_cast<uint8_t>(x), static_cast<uint8_t>(y));
					}
				}
			}
		}
		break;

	case 12:
		{
			for (unsigned int y=0; y<h; y++)
			{
				for (unsigned int x=0; x<w; x++)
				{
					if (((x/4) % 2) == ((y/4) % 2))
					{
						display->setPixel(static_cast<uint8_t>(x), static_cast<uint8_t>(y));
					}
				}
			}
		}
		break;
	case 13:
		{
			for (unsigned int y=0; y<h; y++)
			{
				for (unsigned int x=0; x<w; x++)
				{
					if (((x/4) % 2) != ((y/4) % 2))
					{
						display->setPixel(static_cast<uint8_t>(x), static_cast<uint8_t>(y));
					}
				}
			}
		}
		break;

	default:
		animationCycle = 0;
		return;
	}

	display->sendBuffer();	
	animationCycle++;
}
//---------------------------------------------------------------------------

