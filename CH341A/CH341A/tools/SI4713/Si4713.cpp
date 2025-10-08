/*!
 *  @mainpage Adafruit Si4713 breakout
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for Si4713 breakout
 *
 * 	This is a library for the Adafruit Si4713 breakout:
 * 	http://www.adafruit.com/products/1958
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 */

#include "Si4713.h"
#include "CH341A.h"
#include "Log.h"
#include <string.h>
#include <algorithm>


#define SI4710_STATUS_CTS 0x80 ///< read status

/* COMMANDS */
#define SI4710_CMD_POWER_UP		0x01 ///< Power up device and mode selection. Modes include FM transmit and analog/digital audio interface configuration.
#define SI4710_CMD_GET_REV 		0x10 ///< Returns revision information on the device.
#define SI4710_CMD_POWER_DOWN 	0x11     ///< Power down device
#define SI4710_CMD_SET_PROPERTY 0x12   ///< Sets the value of a property.
#define SI4710_CMD_GET_PROPERTY 0x13   ///< Retrieves a property’s value.
#define SI4710_CMD_GET_INT_STATUS 0x14 ///< Read interrupt status bits.
#define SI4710_CMD_PATCH_ARGS 	0x15 ///< Reserved command used for patch file downloads.
#define SI4710_CMD_PATCH_DATA 	0x16 ///< Reserved command used for patch file downloads.
#define SI4710_CMD_TX_TUNE_FREQ 0x30 ///< Tunes to given transmit frequency.
#define SI4710_CMD_TX_TUNE_POWER 0x31 ///< Sets the output power level and tunes the antenna capacitor
#define SI4710_CMD_TX_TUNE_MEASURE	0x32 ///< Measure the received noise level at the specified frequency.
#define SI4710_CMD_TX_TUNE_STATUS 	0x33 ///< Queries the status of a previously sent TX Tune Freq, TX Tune Power, or TX Tune Measure command.
#define SI4710_CMD_TX_ASQ_STATUS	0x34 ///< Queries the TX status and input audio signal metrics.
#define SI4710_CMD_TX_RDS_BUFF		0x35 ///< Queries the status of the RDS Group Buffer and loads new data into buffer.
#define SI4710_CMD_TX_RDS_PS 		0x36 ///< Set up default PS strings
#define SI4710_CMD_GPO_CTL 			0x80   ///< Configures GPO3 as output or Hi-Z.
#define SI4710_CMD_GPO_SET 			0x81   ///< Sets GPO3 output level (low or high).

/* Parameters */
#define SI4713_PROP_GPO_IEN 0x0001 ///< Enables interrupt sources.
#define SI4713_PROP_DIGITAL_INPUT_FORMAT  0x0101 ///< Configures the digital input format.
#define SI4713_PROP_DIGITAL_INPUT_SAMPLE_RATE 0x0103 ///< Configures the digital input sample rate in 10 Hz steps. Default is 0.
#define SI4713_PROP_REFCLK_FREQ 0x0201 ///< Sets frequency of the reference clock in Hz. The range is 31130 to 34406 Hz, or 0 to disable the AFC. Default is 32768 Hz.
#define SI4713_PROP_REFCLK_PRESCALE 0x0202 ///< Sets the prescaler value for the reference clock.
#define SI4713_PROP_TX_COMPONENT_ENABLE 0x2100 ///< Enable transmit multiplex signal components. Default has pilot and L-R enabled.
#define SI4713_PROP_TX_AUDIO_DEVIATION 0x2101 ///< Configures audio frequency deviation level. Units are in 10 Hz increments. Default is 6285 (68.25 kHz).
#define SI4713_PROP_TX_PILOT_DEVIATION 0x2102 ///< Configures pilot tone frequency deviation level. Units are in 10 Hz increments. Default is 675 (6.75 kHz)
#define SI4713_PROP_TX_RDS_DEVIATION 0x2103 ///< Configures the RDS/RBDS frequency deviation level. Units are in 10 Hz increments. Default is 2 kHz.
#define SI4713_PROP_TX_LINE_LEVEL_INPUT_LEVEL 0x2104 ///< Configures maximum analog line input level to the LIN/RIN pins to reach the maximum deviation level pro- grammed into the audio deviation property TX Audio Deviation. Default is 636 mVPK.
#define SI4713_PROP_TX_LINE_INPUT_MUTE 0x2105 ///< Sets line input mute. L and R inputs may be indepen- dently muted. Default is not muted.
#define SI4713_PROP_TX_PREEMPHASIS 0x2106 ///< Configures pre-emphasis time constant. Default is 0 (75 μS).
#define SI4713_PROP_TX_PILOT_FREQUENCY 0x2107 ///< Configures the frequency of the stereo pilot. Default is 19000 Hz.
#define SI4713_PROP_TX_ACOMP_ENABLE 0x2200 ///< Enables audio dynamic range control. Default is 0 (disabled).
#define SI4713_PROP_TX_ACOMP_THRESHOLD 0x2201 ///< Sets the threshold level for audio dynamic range control. Default is –40 dB.
#define SI4713_PROP_TX_ATTACK_TIME 0x2202 ///< Sets the attack time for audio dynamic range control. Default is 0 (0.5 ms).
#define SI4713_PROP_TX_RELEASE_TIME 0x2203 ///< Sets the release time for audio dynamic range control. Default is 4 (1000 ms).
#define SI4713_PROP_TX_ACOMP_GAIN 0x2204 ///< Sets the gain for audio dynamic range control. Default is 15 dB.
#define SI4713_PROP_TX_LIMITER_RELEASE_TIME 0x2205 ///< Sets the limiter release time. Default is 102 (5.01 ms)
#define SI4713_PROP_TX_ASQ_INTERRUPT_SOURCE 0x2300 ///< Configures measurements related to signal quality metrics. ///< Default is none selected.
#define SI4713_PROP_TX_ASQ_LEVEL_LOW 0x2301 ///< Configures low audio input level detection threshold. This threshold can be used to detect silence on the incoming audio.
#define SI4713_PROP_TX_ASQ_DURATION_LOW 0x2302 ///< Configures the duration which the input audio level must be below the low threshold in order to detect a low audio condition.
#define SI4713_PROP_TX_AQS_LEVEL_HIGH 0x2303 ///< Configures high audio input level detection threshold. This threshold can be used to detect activity on the incoming audio.
#define SI4713_PROP_TX_AQS_DURATION_HIGH 0x2304 ///< Configures the duration which the input audio level must be above the high threshold in order to detect a high audio condition.
#define SI4713_PROP_TX_RDS_INTERRUPT_SOURCE 0x2C00 ///< Configure RDS interrupt sources. Default is none selected.
#define SI4713_PROP_TX_RDS_PI 0x2C01 ///< Sets transmit RDS program identifier.
#define SI4713_PROP_TX_RDS_PS_MIX 0x2C02 ///< Configures mix of RDS PS Group with RDS Group Buffer.
#define SI4713_PROP_TX_RDS_PS_MISC 0x2C03 ///< Miscellaneous bits to transmit along with RDS_PS Groups.
#define SI4713_PROP_TX_RDS_PS_REPEAT_COUNT 0x2C04 ///< Number of times to repeat transmission of a PS message before transmitting the next PS message.
#define SI4713_PROP_TX_RDS_MESSAGE_COUNT 0x2C05 ///< Number of PS messages in use.
#define SI4713_PROP_TX_RDS_PS_AF 0x2C06 ///< RDS Program Service Alternate Frequency. This provides the ability to inform the receiver of a single alternate frequency using AF Method A coding and is transmitted along with the RDS_PS Groups.
#define SI4713_PROP_TX_RDS_FIFO_SIZE 0x2C07 ///< Number of blocks reserved for the FIFO. Note that the value written must be one larger than the desired FIFO size.



Si4713::Si4713(void)
{
}

/*!
 *    @brief  Setups the i2c and calls powerUp function.
 *    @param  addr i2c address
 *    @return True if initialization was successful, otherwise false.
 */
bool Si4713::begin(uint8_t addr, enum Preemphasis preemphasis)
{
  this->addr = addr;
  reset();

  powerUp();

  setProperty(SI4713_PROP_TX_PREEMPHASIS, preemphasis);

  // check for Si4713
  if (getRev() != 13)
    return false;

  return true;
}

/** \brief  Resets the registers to default settings and puts chip in powerdown mode
 */
void Si4713::reset() {
	enum { PIN_RST_TXD = 8 };
	uint32_t direction = (1u << PIN_RST_TXD);

	uint32_t value = 0;
	Sleep(10);
	ch341a.SetGpioOutputs(direction, value);
	Sleep(10);
	value = (1u << PIN_RST_TXD);
	ch341a.SetGpioOutputs(direction, value);
	Sleep(50);
}

/*!
 *    @brief  Set chip property over I2C
 *    @param  property property that will be set
 *    @param  value value of property
 */
void Si4713::setProperty(uint16_t property, uint16_t value) {
  _i2ccommand[0] = SI4710_CMD_SET_PROPERTY;
  _i2ccommand[1] = 0;
  _i2ccommand[2] = static_cast<uint8_t>(property >> 8);
  _i2ccommand[3] = static_cast<uint8_t>(property & 0xFF);
  _i2ccommand[4] = static_cast<uint8_t>(value >> 8);
  _i2ccommand[5] = static_cast<uint8_t>(value & 0xFF);
  sendCommand(6);
}

/*!
 *    @brief  Send command stored in _i2ccommand to chip.
 *    @param  len length of command that will be send
 */
int Si4713::sendCommand(uint8_t len) {
  int status;
  status = ch341a.I2CWriteBytes(addr, _i2ccommand, len);
  if (status)
	return status;

  // Wait for status CTS bit
  uint8_t statusReg = 0;
  for (int i=0; i<20; i++) {
	status = ch341a.I2CReadByte(addr, statusReg);
	if (status)
		return status;
	if (statusReg & SI4710_STATUS_CTS)
		return 0;
	Sleep(5);
  }
  LOG("Si4713 sendCommand: timeout!\n");
  return -1;
}

/*!
 *    @brief  Tunes to given transmit frequency.
 *    @param  freqKHz frequency in KHz
 */
void Si4713::tuneFM(uint16_t freqKHz) {
  _i2ccommand[0] = SI4710_CMD_TX_TUNE_FREQ;
  _i2ccommand[1] = 0;
  _i2ccommand[2] = static_cast<uint8_t>(freqKHz >> 8);
  _i2ccommand[3] = static_cast<uint8_t>(freqKHz & 0xFF);
  sendCommand(4);
  for (int i=0; i<20; i++) {
	if ((getStatus() & 0x81) == 0x81)
		return;
	Sleep(10);
  }
  LOG("Si4713: tuning timeout!\n");
}

/*!
 *    @brief  Sets the output power level and tunes the antenna capacitor
 *    @param  pwr power value
 *    @param  antcap antenna capacitor (default to 0)
 */
void Si4713::setTXpower(uint8_t pwr, uint8_t antcap) {
  _i2ccommand[0] = SI4710_CMD_TX_TUNE_POWER;
  _i2ccommand[1] = 0;
  _i2ccommand[2] = 0;
  _i2ccommand[3] = pwr;
  _i2ccommand[4] = antcap;
  sendCommand(5);
}
/*!
 *    @brief  Queries the TX status and input audio signal metrics.
 */
void Si4713::readASQ() {
  _i2ccommand[0] = SI4710_CMD_TX_ASQ_STATUS;
  _i2ccommand[1] = 0x1;
  sendCommand(2);

  uint8_t resp[5];
  ch341a.I2CReadBytes(addr, resp, sizeof(resp));
  currASQ = resp[1];
  currInLevel = resp[4];
}

/*!
 *    @brief  Queries the status of a previously sent TX Tune Freq, TX Tune
 * Power, or TX Tune Measure using SI4710_CMD_TX_TUNE_STATUS command.
 */
void Si4713::readTuneStatus() {
  _i2ccommand[0] = SI4710_CMD_TX_TUNE_STATUS;
  _i2ccommand[1] = 0x1; // INTACK
  sendCommand(2);

  uint8_t resp[8];
  ch341a.I2CReadBytes(addr, resp, sizeof(resp));
  currFreq = static_cast<uint16_t>((uint16_t(resp[2]) << 8) | resp[3]);
  currdBuV = resp[5];
  currAntCap = resp[6];
  currNoiseLevel = resp[7];
}

/*!
 *    @brief  Measure the received noise level at the specified frequency using SI4710_CMD_TX_TUNE_MEASURE command.
 *    @param  freq frequency
 */
void Si4713::readTuneMeasure(uint16_t freq) {
  // check freq is multiple of 50khz
  if (freq % 5 != 0) {
    freq -= static_cast<uint16_t>(freq % 5);
  }
  // Serial.print("Measuring "); Serial.println(freq);
  _i2ccommand[0] = SI4710_CMD_TX_TUNE_MEASURE;
  _i2ccommand[1] = 0;
  _i2ccommand[2] = static_cast<uint8_t>(freq >> 8);
  _i2ccommand[3] = static_cast<uint8_t>(freq & 0xFF);
  _i2ccommand[4] = 0;

  sendCommand(5);
  for (int i=0; i<20; i++) {
	  if (getStatus() == 0x81)
		break;
	  Sleep(10);
  }
  LOG("Si4713: SI4710_CMD_TX_TUNE_MEASURE timeout!\n");
}

/*!   @brief  Begin RDS
 *    @param  programID sets SI4713_PROP_TX_RDS_PI to parameter value
 */
void Si4713::beginRDS(uint16_t programID) {
  setProperty(SI4713_PROP_TX_AUDIO_DEVIATION, 6625);  // 66.25KHz (default is 68.25)
  setProperty(SI4713_PROP_TX_RDS_DEVIATION, 200);     // 2KHz (default)
  setProperty(SI4713_PROP_TX_RDS_INTERRUPT_SOURCE, 0x0001); // RDS IRQ
  setProperty(SI4713_PROP_TX_RDS_PI, programID);      // program identifier
  setProperty(SI4713_PROP_TX_RDS_PS_MIX, 0x03);       // 50% mix (default)
  setProperty(SI4713_PROP_TX_RDS_PS_MISC, 0x1008);    // RDSD0 & RDSMS (default)
  setProperty(SI4713_PROP_TX_RDS_PS_REPEAT_COUNT, 3); // 3 repeats (default)
  setProperty(SI4713_PROP_TX_RDS_MESSAGE_COUNT, 1);   // 1 message (default)
  setProperty(SI4713_PROP_TX_RDS_PS_AF, 0xE0E0);      // no AF (default)
  setProperty(SI4713_PROP_TX_RDS_FIFO_SIZE, 0);       // no FIFO (default)
  setProperty(SI4713_PROP_TX_COMPONENT_ENABLE, 0x0007); // enable RDS, stereo, tone
}

/**	\brief  Set up the RDS station string
	\param  s string to load
 */
void Si4713::setRDSstation(const char *s) {
  unsigned int len = strlen(s);
  if (len > 256) {
	LOG("RDS station name too long!");
	return;
  }
  uint8_t slots = static_cast<uint8_t>((len + 3) / 4);

  for (uint8_t i = 0; i < slots; i++) {
    memset(_i2ccommand, ' ', 6); // clear it with ' '
    memcpy(_i2ccommand + 2, s, std::min(4, (int)strlen(s)));
    s += 4;
    _i2ccommand[6] = 0;
#ifdef SI4713_CMD_DEBUG
    Serial.print("Set slot #");
    Serial.print(i);
    char *slot = (char *)(_i2ccommand + 2);
    Serial.print(" to '");
    Serial.print(slot);
    Serial.println("'");
#endif
    _i2ccommand[0] = SI4710_CMD_TX_RDS_PS;
    _i2ccommand[1] = i; // slot #
    sendCommand(6);
  }
}

/**	\brief  Queries the status of the RDS Group Buffer and loads new data into buffer.
	\param  s string to load
 */
void Si4713::setRDSbuffer(const char *s) {
  unsigned int len = strlen(s);
  if (len > 256) {
	LOG("RDS buffer to send too long!");
	return;
  }
  uint8_t slots = static_cast<uint8_t>((len + 3) / 4);

  for (uint8_t i = 0; i < slots; i++) {
    memset(_i2ccommand, ' ', 8); // clear it with ' '
    memcpy(_i2ccommand + 4, s, std::min(4, (int)strlen(s)));
    s += 4;
    _i2ccommand[8] = 0;
#ifdef SI4713_CMD_DEBUG
    Serial.print("Set buff #");
    Serial.print(i);
    char *slot = (char *)(_i2ccommand + 4);
    Serial.print(" to '");
    Serial.print(slot);
    Serial.println("'");
#endif
    _i2ccommand[0] = SI4710_CMD_TX_RDS_BUFF;
    if (i == 0)
      _i2ccommand[1] = 0x06;
    else
      _i2ccommand[1] = 0x04;

    _i2ccommand[2] = 0x20;
    _i2ccommand[3] = i;
    sendCommand(8);
  }
}

/*!
 *    @brief  Read interrupt status bits.
 *    @return status bits
 */
uint8_t Si4713::getStatus() {
  uint8_t val = 0;
  ch341a.I2CWriteCommandReadByte(addr, SI4710_CMD_GET_INT_STATUS, val);
  return val;
}

void Si4713::powerUp() {
  _i2ccommand[0] = SI4710_CMD_POWER_UP;
  _i2ccommand[1] = 0x12;
  // CTS interrupt disabled
  // GPO2 output disabled
  // Boot normally
  // xtal oscillator ENabled
  // FM transmit
  _i2ccommand[2] = 0x50; // analog input mode
  sendCommand(3);

  // configuration! see page 254
  setProperty(SI4713_PROP_REFCLK_FREQ, 32768); // crystal is 32.768
  setProperty(SI4713_PROP_TX_ACOMP_GAIN, 10);  // max gain?
  // setProperty(SI4713_PROP_TX_ACOMP_ENABLE, 0x02); // turn on limiter, but no
  // dynamic ranging
  setProperty(SI4713_PROP_TX_ACOMP_ENABLE, 0x0); // turn on limiter and AGC
}

/*!
 *    @brief  Get the hardware revision code from the device using SI4710_CMD_GET_REV
 *    @return revision number
 */
uint8_t Si4713::getRev() {
  _i2ccommand[0] = SI4710_CMD_GET_REV;
  _i2ccommand[1] = 0;
  sendCommand(2);

  uint8_t pn, resp[9];
  ch341a.I2CReadBytes(addr, resp, sizeof(resp));
  pn = resp[1];

#ifdef SI4713_CMD_DEBUG
  uint8_t fw, patch, cmp, chiprev;
  fw = (uint16_t(resp[2]) << 8) | resp[3];
  patch = (uint16_t(resp[4]) << 8) | resp[5];
  cmp = (uint16_t(resp[6]) << 8) | resp[7];
  chiprev = resp[8];

  Serial.print("Part # Si47");
  Serial.print(pn);
  Serial.print("-");
  Serial.println(fw, HEX);
  Serial.print("Firmware 0x");
  Serial.println(fw, HEX);
  Serial.print("Patch 0x");
  Serial.println(patch, HEX);
  Serial.print("Chip rev ");
  Serial.write(chiprev);
  Serial.println();
#endif

  return pn;
}

/*!
 *    @brief  Configures GP1 / GP2 as output or Hi-Z.
 *    @param  x bit value
 */
void Si4713::setGPIOctrl(uint8_t x) {
  _i2ccommand[0] = SI4710_CMD_GPO_CTL;
  _i2ccommand[1] = x;
  sendCommand(2);
}

/*!
 *    @brief  Sets GP1 / GP2 output level (low or high).
 *    @param  x bit value
 */
void Si4713::setGPIO(uint8_t x) {
  _i2ccommand[0] = SI4710_CMD_GPO_SET;
  _i2ccommand[1] = x;
  sendCommand(2);
}
