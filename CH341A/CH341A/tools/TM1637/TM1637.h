//---------------------------------------------------------------------------
#ifndef TM1637H
#define TM1637H
//---------------------------------------------------------------------------

#include <stdint.h>

class TM1637
{
public:
    void init(void);

	enum Brightness {
		Brightness_01_16 = 0,	// 1/16 pulse width
		Brightness_02_16,
		Brightness_04_16,
		Brightness_10_16,
		Brightness_11_16,
		Brightness_12_16,
		Brightness_13_16,
		Brightness_14_16,
		Brightness_Limiter
	};
	static const char* getBrightnessName(enum Brightness brightness);
	int setBrightness(enum Brightness brightness);

	enum { SYMBOL_COUNT = 6 };
	enum { DECIMAL_POINT = 0x80000000 };
	enum { SYMBOL_CHARACTER_MASK = 0xFF };
	/** \param symbols array of SYMBOL_COUNT size */
	int writeSymbols(unsigned int *symbols);

private:
    bool writeByte(uint8_t value);
	void start(void);
    void stop(void);
};

#endif
