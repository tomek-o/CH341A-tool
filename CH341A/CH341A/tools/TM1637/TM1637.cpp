//---------------------------------------------------------------------------
#pragma hdrstop
#include "TM1637.h"
#include "CH341A.h"
#include "Log.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

namespace
{

enum
{
    SCLK_PIN_ID = 18,		// SCLK = SCL, output only
    DATA_OUT_PIN_ID = 19,	// IO = SDA, semi-bidirectional with open-drain output and pull-up
    DATA_IN_PIN_ID = 23		// IO = SDA, separate bit required for reading input
};

bool sclkState = false;
bool ioDirOut = false;
bool ioState = false;

void update(void)
{
    uint32_t direction = (1u << SCLK_PIN_ID);
    if (ioDirOut)
        direction |= (1u << DATA_OUT_PIN_ID);
    uint32_t value = 0;
    if (sclkState)
        value |= (1u << SCLK_PIN_ID);
    if (ioState)
        value |= (1u << DATA_OUT_PIN_ID);

    ch341a.SetGpioOutputs(direction, value);
}

void setClk(bool state)
{
    sclkState = state;
    update();
}

void setIoDirOut(bool state)
{
    ioDirOut = state;
    update();
}

void setIoState(bool state)
{
    ioState = state;
    update();
}

uint8_t getIoState(void)
{
    uint32_t dataIn = 0;

    int status = ch341a.GetGpioInputs(dataIn);
    if (status != 0)
    {
		LOG("Reading inputs failed!\n");
        return 0;
    }
    if (dataIn & (1u << DATA_IN_PIN_ID))
        return 1;
    return 0;
}

void bitDelay(void)
{
    Sleep(1);
}

uint8_t SymbolToSegments(unsigned int symbol)
{
	uint8_t c = static_cast<uint8_t>(symbol & TM1637::SYMBOL_CHARACTER_MASK);
	uint8_t ret = 0;

    c = static_cast<uint8_t>(toupper(c));

	switch (c)
	{
	case '0':
		ret = 0x3F;
		break;
	case '1':
		ret = 0x06;
		break;
	case '2':
		ret = 0x5B;
		break;
	case '3':
		ret = 0x4F;
		break;
	case '4':
		ret = 0x66;
		break;
	case '5':
		ret = 0x6D;
		break;
	case '6':
		ret = 0x7D;
		break;
	case '7':
		ret = 0x07;
		break;
	case 8:
		ret = 0x7F;
		break;
	case '9':
		ret = 0x6F;
		break;
	case 'A':
		ret = 0x77;
		break;
	case 'B':
		ret = 0x7C;
		break;
	case 'C':
		ret = 0x39;
		break;
	case 'D':
		ret = 0x5E;
		break;
	case 'E':
		ret = 0x79;
		break;
	case 'F':
		ret = 0x71;
		break;
	default:
		break;
	}

	if (symbol & TM1637::DECIMAL_POINT)
		ret |= 0x80;
	return ret;
}

}	// namespace


const char* TM1637::getBrightnessName(enum Brightness brightness)
{
	switch (brightness)
	{
	case Brightness_01_16:
		return "1/16 pulse width (lowest)";
	case Brightness_02_16:
		return "2/16 pulse width";
	case Brightness_04_16:
		return "4/16 pulse width";
	case Brightness_10_16:
		return "10/16 pulse width";
	case Brightness_11_16:
		return "11/16 pulse width";
	case Brightness_12_16:
		return "12/16 pulse width";
	case Brightness_13_16:
		return "13/16 pulse width";
	case Brightness_14_16:
		return "14/16 pulse width (highest)";
	default:
		return "???";
	}
}


void TM1637::init(void)
{
    sclkState = false;
    ioDirOut = false;
    ioState = false;
    update();
}

bool TM1637::writeByte(uint8_t value)
{
    for(int i=0; i<8; i++)
    {
        setClk(false);
		setIoState(value & 0x01);
        value >>= 1;
        setClk(true);
    }
	setClk(false);
    setIoState(true);
    setClk(true);
    setIoDirOut(false);

    bitDelay();
	bool ack = (getIoState() == 0);	// ACK = pulling low by TM1637
    if (ack == false)
    {
        setIoDirOut(true);
        setIoState(false);
    }
    bitDelay();
    setIoDirOut(true);
    bitDelay();

	return ack;
}

void TM1637::start(void)
{
	setClk(true);
    setIoState(true);
    setIoState(false);
    setClk(false);
}

void TM1637::stop(void)
{
    setClk(false);
    setIoState(false);
    setClk(true);
    setIoState(true);
}

int TM1637::writeSymbols(unsigned int *symbols)
{
	bool ack;
	start();
	ack = writeByte(0x40);		// auto-increment address
	stop();
	if (!ack)
		return -1;
	start();
	ack = writeByte(0xC0);		// starting address
	if (ack)
	{
		for(unsigned i=0; i < SYMBOL_COUNT; i ++)
		{
			ack = writeByte(SymbolToSegments(symbols[i]));
			if (!ack)
				break;
		}
	}
	stop();
	return ack ? 0 : -1;
}

int TM1637::setBrightness(enum Brightness brightness)
{
	start();
	/// \note BIT 3 = ON/OFF
	bool ack = writeByte(static_cast<uint8_t>(0x88 + brightness));
	stop();
	return ack ? 0 : -1;
}


