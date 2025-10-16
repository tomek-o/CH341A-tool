/** \file
	\brief Changelog
*/

/** \page changelog Changelog

Version 1.0 - first release
- detecting I2C devices
- writing/reading raw I2C data
- reading Smart Battery information
- writing/reading CH341A GPIO
- reading data from some popular I2C sensors:
	- BME280
	- BMP180
	- Si7021
- writing to SSD1306-based OLED display
- controlling RDA5807M FM radio module

Version 1.1
- added reading temperature from LM75 I2C sensor
- added writing to HD44780 display using GPIO
- added writing to HD44780 + PCF8574 (I2C) display
- added I2C address selection to BME280
- BMP280 is listed next to BME280
- added AHT10/AHT20 temperature + humidity sensor
- added writing/reading raw SPI data (but CH341 SPI is quite limited)
- added writing to SPI MAX7219 8x8 LED matrix display
- added AS5600 - magnetic rotary position sensor with 12-bit resolution

Version 1.2
- added Si5351A clock generator module
- added VL53L0X ToF distance sensor reading
- added APDS-9960 light/proximity/gesture sensor reading
- added L3G4200D gyro reading

Version 1.3
- added MCP4725 DAC
- added BMI160 IMU
- added MPU6050 IMU
- added simple line and square shapes generator (whole screen area test) to SSD1306 test
- MDIO interface, using UART TX (as MDC) and RX (as MDIO) CH341A pins 
	- device detection (PHY address + ID detection) window
	- Clause 22 read/write window with bit descriptions for basic and extended PHY registers
	- Clause 22 for clause 45 (MMD) read/write window
- added LAN8742A MDIO TDR (Time Domain Reflectometry) cable diagnostic over MDIO interface
- added LAN8742A MDIO estimated cable length reading for matched cable with 100 Mbps active link
- added option to start each log line with timestamp

Version 1.4
- added DS1302 RTC
- added DS1307 RTC
- added Sensirion SHT11 sensor
- added CH455 LED and keypad I2C controller
- added ADS1115 ADC
- added audio beep options for I2C and MDIO detection windows

Version 1.5
- CH341 is reopened automatically if I2C clock speed was changed in settings
- modules can check CH341 I2C clock speed (new discovery: CH341 probably does not support clock stretching, it seems to be added in CH347)
- added MAX11601 ADC
- added TEA5767 FM tuner
- added S35390A RTC from ABLIC Inc.
- added DS3231 RTC
- added ST7567 SPI LCD display
- added cascaded MAX7219 4x8x8 LED display
- added HMC5883L magnetometer/compass
- added INA226 power monitor
- I2C device detection window: added option to use i2cdetect-like formatting for output instead of list

Version 1.6
- BREAKING: changed pins used for MDIO from TXD + RXD to (safer) SCL + SDA
- I2C detect: added option to limit address range to make oscilloscope triggering easier
- I2C detect: added setting for auto repeat interval time
- added Si4713 FM radio transmitter
- added VEML7700 ambient light sensor
- added CH1115 48x88px 0.5" OLED display
*/
