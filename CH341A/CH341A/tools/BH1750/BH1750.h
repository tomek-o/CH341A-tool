/*
MIT License

Copyright (c) 2026 SimoSbara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef BH1750H
#define BH1750H

#include <stdint.h>

#define BH1750_REG			(0x23) //Address

#define BH1750_CMD_ON   		(0x01) //Power ON
#define BH1750_CMD_OFF  		(0x00) //Power OFF
#define BH1750_CMD_RESET  	(0x07) //Reset registry after Power ON

#define BH1750_HRES1   		(0x00) //High Resolution 1 (1 lx resolution - 120 ms)
#define BH1750_HRES2  		(0x01) //High Resolution 2 (0.5 lx resolution - 120 ms)
#define BH1750_LRES  		(0x03) //Low Resolution (4 lx resolution - 16 ms)

#define BH1750_CONTINUE_MODE	(0x10)
#define BH1750_SINGLE_MODE	(0x20)

class BH1750 {
public:
  BH1750(void);
  ~BH1750();

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
