//by SimoSbara
#include <vcl.h>
#pragma hdrstop

#include "BH1750FVI.h"
#include "CH341A.h"
#include <stdlib.h>

#ifdef __BORLANDC__
#pragma warn -8071
#endif

BH1750FVI::BH1750FVI(void)
{
  _initialized = false;
}

BH1750FVI::~BH1750FVI()
{
}

bool BH1750FVI::open(bool init)
{
  if(!write(BH1750FVI_CMD_ON))
	return false;

  if(init)
	if(!write(BH1750FVI_CMD_RESET))
		return false;

  _initialized = true;
  
  return true;
}

bool BH1750FVI::close()
{
  if(!write(BH1750FVI_CMD_OFF))
	return false;

  _initialized = false;
  
  return true;
}

bool BH1750FVI::start(uint8_t mode, bool single)
{
	if(!isInitialized())
		return false;

	bool res = false;

	uint8_t opcode = mode |
					((single) ? BH1750FVI_SINGLE_MODE :
								BH1750FVI_CONTINUE_MODE);

	res = write(opcode);

	if(single)
		open(false);

    return res;
}

float BH1750FVI::calculateLux(uint16_t value)
{
  return static_cast<float>(value) / 1.2F;
}

uint16_t BH1750FVI::getLuminosity()
{
	if(!isInitialized())
		return false;

	return read();
}

uint16_t BH1750FVI::read()
{
  uint8_t buffer[2];
  ch341a.I2CReadBytes(BH1750FVI_REG, buffer, sizeof(buffer));
  return uint16_t((buffer[1]) << 8) | uint16_t(buffer[0]);
}

bool BH1750FVI::write(uint8_t value)
{
  return !ch341a.I2CWriteByte(BH1750FVI_REG, value);
}
