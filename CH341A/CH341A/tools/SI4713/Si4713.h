#ifndef Si4713H
#define Si4713H

/*!
 *  This is a library for the Si4713 breakout
 *
 *  Designed specifically to work with the Adafruit Si4713 breakout
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/1958
 *
 *  These transmitters use I2C to communicate, plus reset pin,
 *  3 pins are required to interface
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 *  BSD license, all text above must be included in any redistribution
 *
 *  Adapted to CH341A/Windows/Turbo C++ by Tomasz Ostrowski.
 */

#include <stdint.h>

#define SI4710_ADDR0 0x11      ///< if SEN is low
#define SI4710_ADDR1 0x63      ///< if SEN is high, default!

class Si4713 {
public:
  Si4713(void);

  enum Preemphasis {
	PREEMPHASIS_US_75us = 0,
	PREEMPHASIS_EU_50us,
	PREEMPHASIS_DISABLED,
  };

  bool begin(uint8_t addr, enum Preemphasis preemphasis);
  void reset();

  void powerUp();
  uint8_t getRev();

  void tuneFM(uint16_t freqKHz);
  uint8_t getStatus(void);
  int readTuneStatus(void);
  int readTuneMeasure(uint16_t freq);
  void setTXpower(uint8_t pwr, uint8_t antcap = 0);
  void readASQ(void);
  void setProperty(uint16_t p, uint16_t v);

  // RDS stuff
  void beginRDS(uint16_t programID = 0xADAF);
  void setRDSstation(const char *s);
  void setRDSbuffer(const char *s);

  uint16_t currFreq;  ///< current frequency
  uint8_t currdBuV,   ///< received noise level (RNL) in units of dBuV on the selected frequency
      currAntCap,     ///< current antenna capacitor
      currNoiseLevel, ///< current noise level
      currASQ;        ///< current ASQ (Audio Signal Quality)
  int8_t currInLevel; ///< current IN level

  void setGPIO(uint8_t x);
  void setGPIOctrl(uint8_t x);

private:
  int sendCommand(uint8_t len);

  uint8_t addr;
  uint8_t _i2ccommand[10]; // holds the command buffer
};

#endif
