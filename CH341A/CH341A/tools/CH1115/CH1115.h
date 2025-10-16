#ifndef CH1115H
#define CH1115H

#include <string.h>
#include <stdint.h>

class CH341A;

class CH1115 {

public:
	enum { WIDTH = 88, HEIGHT = 48 };
	
	CH1115(CH341A &ch341a, uint8_t Address);

	/// \param orientation - 0 for not flipped, 1 for flipped display
	void setOrientation(bool orientation);

	void clear(void);

	void data(unsigned char val);
	void data(const unsigned char *buf, unsigned int count);

	void setColumn(unsigned char column);
	void setPage(unsigned char page);

	/// \brief Inverts screen on hardware level. Way more efficient than setting buffer to all ones and then using WriteMode subtract.
	void invertDisplay();

	/// \param contrast - accepted values of 0 to 255 to set the contrast
	void setContrast(unsigned char contrast);

	void turnOff();

	void turnOn();

private:
	CH341A &ch341a;
	uint8_t address;

	bool inverted;

	/// \brief Sends single 8bit command
	/// \param command - byte to be sent to controller
	void cmd(unsigned char command);
};

#endif