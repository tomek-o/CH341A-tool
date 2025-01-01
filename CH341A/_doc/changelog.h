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
*/
