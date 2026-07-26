//by SimoSbara

#ifndef BH1750FVIH
#define BH1750FVIH

#include <stdint.h>

#define BH1750FVI_REG			(0x23) //Address

#define BH1750FVI_CMD_ON   		(0x01) //Power ON
#define BH1750FVI_CMD_OFF  		(0x00) //Power OFF
#define BH1750FVI_CMD_RESET  	(0x07) //Reset registry after Power ON

#define BH1750FVI_HRES1   		(0x00) //High Resolution 1 (1 lx resolution - 120 ms)
#define BH1750FVI_HRES2  		(0x01) //High Resolution 2 (0.5 lx resolution - 120 ms)
#define BH1750FVI_LRES  		(0x03) //Low Resolution (4 lx resolution - 16 ms)

#define BH1750FVI_CONTINUE_MODE	(0x10)
#define BH1750FVI_SINGLE_MODE	(0x20)

/**************************************************************************/
/*!
	@brief  Class that stores state and functions for interacting with TSL2591
   Light Sensor
*/
/**************************************************************************/
class BH1750FVI {
public:
  BH1750FVI(void);
  ~BH1750FVI();

  bool open(bool init = true);
  bool close(void);
  
  bool start(uint8_t mode, bool single);
  float calculateLux(uint16_t value);
  uint16_t getLuminosity();

  bool isInitialized(void) const {
  	return _initialized;
  }

private:
  bool write(uint8_t v);
  uint16_t read();

  bool _initialized;
};
#endif
