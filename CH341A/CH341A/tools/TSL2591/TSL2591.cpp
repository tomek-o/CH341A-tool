/**************************************************************************/
/*!
	@author   KT0WN (adafruit.com)
	Modified for CH341A-tool.

    This is a library for the Adafruit TSL2591 breakout board
    This library works with the Adafruit TSL2591 breakout
    ----> https://www.adafruit.com/products/1980

    Check out the links above for our tutorials and wiring diagrams
    These chips use I2C to communicate

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2014 Adafruit Industries
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "TSL2591.h"
#include "CH341A.h"
#include <stdlib.h>

#ifdef __BORLANDC__
#pragma warn -8071
#endif

namespace
{
	void SleepWithMsgProcessing(int ms)
	{
		do
		{
			Sleep(20);
			Application->ProcessMessages();
			ms -= 20;
		} while (ms > 0);
	}
}

/**************************************************************************/
/*!
    @brief  Instantiates a new Adafruit TSL2591 class
    @param  sensorID An optional ID # so you can track this sensor, it will tag
   sensorEvents you create.
*/
/**************************************************************************/
Adafruit_TSL2591::Adafruit_TSL2591(void) {
  _initialized = false;
  _integration = TSL2591_INTEGRATIONTIME_100MS;
  _gain = TSL2591_GAIN_MED;
}

Adafruit_TSL2591::~Adafruit_TSL2591() {
}

/**************************************************************************/
/*!
    @brief   Setups the I2C interface and hardware, identifies if chip is found
    @param   theWire a reference to TwoWire instance
    @param   addr The I2C adress of the sensor (Default 0x29)
    @returns True if a TSL2591 is found, false on any failure
*/
/**************************************************************************/
bool Adafruit_TSL2591::begin(void) {
  /*
  for (uint8_t i=0; i<0x20; i++)
  {
    uint8_t id = read8(0x12);
    Serial.print("$"); Serial.print(i, HEX);
    Serial.print(" = 0x"); Serial.println(read8(i), HEX);
  }
  */

  uint8_t id = read8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_DEVICE_ID);
  if (id != 0x50) {
    return false;
  }
  // Serial.println("Found Adafruit_TSL2591");

  _initialized = true;

  // Set default integration time and gain
  setTiming(_integration);
  setGain(_gain);

  // Note: by default, the device is in power down mode on bootup
  disable();

  return true;
}

/**************************************************************************/
/*!
    @brief  Enables the chip, so it's ready to take readings
*/
/**************************************************************************/
void Adafruit_TSL2591::enable(void) {
  if (!_initialized) {
    if (!begin()) {
      return;
    }
  }

  // Enable the device by setting the control bit to 0x01
  write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_ENABLE,
         TSL2591_ENABLE_POWERON | TSL2591_ENABLE_AEN | TSL2591_ENABLE_AIEN |
             TSL2591_ENABLE_NPIEN);
}

/**************************************************************************/
/*!
    @brief Disables the chip, so it's in power down mode
*/
/**************************************************************************/
void Adafruit_TSL2591::disable(void) {
  if (!_initialized) {
    if (!begin()) {
      return;
    }
  }

  // Disable the device by setting the control bit to 0x00
  write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_ENABLE,
         TSL2591_ENABLE_POWEROFF);
}

/************************************************************************/
/*!
    @brief  Setter for sensor light gain
    @param  gain {@link tsl2591Gain_t} gain value
*/
/**************************************************************************/
void Adafruit_TSL2591::setGain(tsl2591Gain_t gain) {
  if (!_initialized) {
    if (!begin()) {
      return;
    }
  }

  enable();
  _gain = gain;
  write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CONTROL, _integration | _gain);
  disable();
}

/************************************************************************/
/*!
    @brief  Getter for sensor light gain
    @returns {@link tsl2591Gain_t} gain value
*/
/**************************************************************************/
tsl2591Gain_t Adafruit_TSL2591::getGain() { return _gain; }

/************************************************************************/
/*!
    @brief  Setter for sensor integration time setting
    @param integration {@link tsl2591IntegrationTime_t} integration time setting
*/
/**************************************************************************/
void Adafruit_TSL2591::setTiming(tsl2591IntegrationTime_t integration) {
  if (!_initialized) {
    if (!begin()) {
      return;
    }
  }

  enable();
  _integration = integration;
  write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CONTROL, _integration | _gain);
  disable();
}

/************************************************************************/
/*!
    @brief  Getter for sensor integration time setting
    @returns {@link tsl2591IntegrationTime_t} integration time
*/
/**************************************************************************/
tsl2591IntegrationTime_t Adafruit_TSL2591::getTiming() { return _integration; }

/************************************************************************/
/*!
    @brief  Calculates the visible Lux based on the two light sensors
    @param  ch0 Data from channel 0 (IR+Visible)
    @param  ch1 Data from channel 1 (IR)
    @returns Lux, based on AMS coefficients (or < 0 if overflow)
*/
/**************************************************************************/
float Adafruit_TSL2591::calculateLux(uint16_t ch0, uint16_t ch1) {
  float atime, again;
  float cpl, lux;

  // Check for overflow conditions first
  if ((ch0 == 0xFFFF) | (ch1 == 0xFFFF)) {
    // Signal an overflow
    return -1;
  }

  // Note: This algorithm is based on preliminary coefficients
  // provided by AMS and may need to be updated in the future

  switch (_integration) {
  case TSL2591_INTEGRATIONTIME_100MS:
    atime = 100.0F;
    break;
  case TSL2591_INTEGRATIONTIME_200MS:
    atime = 200.0F;
    break;
  case TSL2591_INTEGRATIONTIME_300MS:
    atime = 300.0F;
    break;
  case TSL2591_INTEGRATIONTIME_400MS:
    atime = 400.0F;
    break;
  case TSL2591_INTEGRATIONTIME_500MS:
    atime = 500.0F;
    break;
  case TSL2591_INTEGRATIONTIME_600MS:
    atime = 600.0F;
    break;
  default: // 100ms
    atime = 100.0F;
    break;
  }

  switch (_gain) {
  case TSL2591_GAIN_LOW:
    again = 1.0F;
    break;
  case TSL2591_GAIN_MED:
    again = 25.0F;
    break;
  case TSL2591_GAIN_HIGH:
    again = 428.0F;
    break;
  case TSL2591_GAIN_MAX:
    again = 9876.0F;
    break;
  default:
    again = 1.0F;
    break;
  }

  // cpl = (ATIME * AGAIN) / DF
  cpl = (atime * again) / TSL2591_LUX_DF;

  // Original lux calculation (for reference sake)
  // float lux1 = ( (float)ch0 - (TSL2591_LUX_COEFB * (float)ch1) ) / cpl;
  // float lux2 = ( ( TSL2591_LUX_COEFC * (float)ch0 ) - ( TSL2591_LUX_COEFD *
  // (float)ch1 ) ) / cpl; lux = lux1 > lux2 ? lux1 : lux2;

  // Alternate lux calculation 1
  // See: https://github.com/adafruit/Adafruit_TSL2591_Library/issues/14
  if (cpl < 1e-6 || ch0 == 0)	// prevent floating point exception on communication error
  {
  	return 0;
  }
  lux = (((float)ch0 - (float)ch1)) * (1.0F - ((float)ch1 / (float)ch0)) / cpl;

  // Alternate lux calculation 2
  // lux = ( (float)ch0 - ( 1.7F * (float)ch1 ) ) / cpl;

  // Signal I2C had no errors
  return lux;
}

/************************************************************************/
/*!
    @brief  Reads the raw data from both light channels
    @returns 32-bit raw count where high word is IR, low word is IR+Visible
*/
/**************************************************************************/
uint32_t Adafruit_TSL2591::getFullLuminosity(void) {
  if (!_initialized) {
    if (!begin()) {
      return 0;
    }
  }

  // Enable the device
  enable();

  // Wait x ms for ADC to complete
  for (uint8_t d = 0; d <= _integration; d++) {
    SleepWithMsgProcessing(120);
  }

  // CHAN0 must be read before CHAN1
  // See: https://forums.adafruit.com/viewtopic.php?f=19&t=124176
  uint32_t x;
  uint16_t y;
  y = read16(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN0_LOW);
  x = read16(TSL2591_COMMAND_BIT | TSL2591_REGISTER_CHAN1_LOW);
  x <<= 16;
  x |= y;

  disable();

  return x;
}

/************************************************************************/
/*!
    @brief  Reads the raw data from the channel
    @param  channel Can be 0 (IR+Visible, 1 (IR) or 2 (Visible only)
    @returns 16-bit raw count, or 0 if channel is invalid
*/
/**************************************************************************/
uint16_t Adafruit_TSL2591::getLuminosity(uint8_t channel) {
  uint32_t x = getFullLuminosity();

  if (channel == TSL2591_FULLSPECTRUM) {
    // Reads two byte value from channel 0 (visible + infrared)
    return (x & 0xFFFF);
  } else if (channel == TSL2591_INFRARED) {
    // Reads two byte value from channel 1 (infrared)
    return (x >> 16);
  } else if (channel == TSL2591_VISIBLE) {
    // Reads all and subtracts out just the visible!
    return ((x & 0xFFFF) - (x >> 16));
  }

  // unknown channel!
  return 0;
}

/************************************************************************/
/*!
    @brief  Set up the interrupt to go off when light level is outside the
   lower/upper range.
    @param  lowerThreshold Raw light data reading level that is the lower value
   threshold for interrupt
    @param  upperThreshold Raw light data reading level that is the higher value
   threshold for interrupt
    @param  persist How many counts we must be outside range for interrupt to
   fire, default is any single value
*/
/**************************************************************************/
void Adafruit_TSL2591::registerInterrupt(
    uint16_t lowerThreshold, uint16_t upperThreshold,
    tsl2591Persist_t persist = TSL2591_PERSIST_ANY) {
  if (!_initialized) {
    if (!begin()) {
      return;
    }
  }

  enable();
  write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_PERSIST_FILTER, persist);
  write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_THRESHOLD_AILTL,
         lowerThreshold);
  write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_THRESHOLD_AILTH,
         lowerThreshold >> 8);
  write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_THRESHOLD_AIHTL,
         upperThreshold);
  write8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_THRESHOLD_AIHTH,
         upperThreshold >> 8);
  disable();
}

/************************************************************************/
/*!
    @brief  Clear interrupt status
*/
/**************************************************************************/
void Adafruit_TSL2591::clearInterrupt() {
  if (!_initialized) {
    if (!begin()) {
      return;
    }
  }

  enable();
  write8(TSL2591_CLEAR_INT);
  disable();
}

/************************************************************************/
/*!
    @brief  Gets the most recent sensor event from the hardware status register.
    @return Sensor status as a byte. Bit 0 is ALS Valid. Bit 4 is ALS Interrupt.
   Bit 5 is No-persist Interrupt.
*/
/**************************************************************************/
uint8_t Adafruit_TSL2591::getStatus(void) {
  if (!_initialized) {
    if (!begin()) {
      return 0;
    }
  }

  // Enable the device
  enable();
  uint8_t x;
  x = read8(TSL2591_COMMAND_BIT | TSL2591_REGISTER_DEVICE_STATUS);
  disable();
  return x;
}

uint8_t Adafruit_TSL2591::read8(uint8_t reg) {
  uint8_t ret = 0xFF;
  ch341a.I2CWriteCommandReadByte(TSL2591_ADDR, reg, ret);
  return ret;
}

uint16_t Adafruit_TSL2591::read16(uint8_t reg) {
  uint8_t buffer[2];
  ch341a.I2CWriteCommandReadBytes(TSL2591_ADDR, reg, buffer, sizeof(buffer));
  return uint16_t((buffer[1]) << 8) | uint16_t(buffer[0]);
}

void Adafruit_TSL2591::write8(uint8_t reg, uint8_t value) {
  ch341a.I2CWriteCommandWriteByte(TSL2591_ADDR, reg, value);
}

void Adafruit_TSL2591::write8(uint8_t reg) {
  ch341a.I2CWriteByte(TSL2591_ADDR, reg);
}
