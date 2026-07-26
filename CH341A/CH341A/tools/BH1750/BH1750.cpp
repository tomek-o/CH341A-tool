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

#include <vcl.h>
#pragma hdrstop

#include "BH1750.h"
#include "CH341A.h"
#include <stdlib.h>

#ifdef __BORLANDC__
#pragma warn -8071
#endif

BH1750::BH1750(void)
{
  _initialized = false;
}

BH1750::~BH1750()
{
}

bool BH1750::open(bool init)
{
  if(!write(BH1750_CMD_ON))
	return false;

  if(init)
	if(!write(BH1750_CMD_RESET))
		return false;

  _initialized = true;
  
  return true;
}

bool BH1750::close()
{
  if(!write(BH1750_CMD_OFF))
	return false;

  _initialized = false;
  
  return true;
}

bool BH1750::start(uint8_t mode, bool single)
{
	if(!isInitialized())
		return false;

	bool res = false;

	uint8_t opcode = mode |
					((single) ? BH1750_SINGLE_MODE :
								BH1750_CONTINUE_MODE);

	res = write(opcode);

	if(single)
		open(false);

    return res;
}

float BH1750::calculateLux(uint16_t value)
{
  return static_cast<float>(value) / 1.2F;
}

uint16_t BH1750::getLuminosity()
{
	if(!isInitialized())
		return false;

	return read();
}

uint16_t BH1750::read()
{
  uint8_t buffer[2];
  ch341a.I2CReadBytes(BH1750_REG, buffer, sizeof(buffer));
  //response from datasheet: High Byte | ACK | Low Byte | !ACK 
  return uint16_t((buffer[0]) << 8) | uint16_t(buffer[1]);
}

bool BH1750::write(uint8_t value)
{
  return !ch341a.I2CWriteByte(BH1750_REG, value);
}
