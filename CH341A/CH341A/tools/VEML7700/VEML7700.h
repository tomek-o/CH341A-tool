/*!
 * @brief  DFRobot's Light Sensor
 * @n      High Accuracy Ambient Light Sensor
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [yangyang](971326313@qq.com)
 * @version  V1.0
 * @date  2016-12-08
 * @url  https://github.com/DFRobot/DFRobot_VEML7700
 * @note Adapted to CH341/Windows
 */
#ifndef VEML7700H
#define VEML7700H

#include <stdint.h>

class VEML7700
{
public:
  enum { I2C_ADDRESS = 0x10 };
  /**
   * @enum  eAlsGain_t
   * @brief  Gain selection
   */
  typedef enum
  {
    ALS_GAIN_x1 = 0x0,   /**< x 1 */
    ALS_GAIN_x2 = 0x1,   /**< x 2 */
    ALS_GAIN_d8 = 0x2,   /**< x 1/8 */
    ALS_GAIN_d4 = 0x3,   /**< x 1/4 */
  }eAlsGain_t;

  /**
   * @enum  eAlsItime_t
   * @brief  ALS integration time setting
   */
  typedef enum
  {
    ALS_INTEGRATION_25ms = 0xc,
    ALS_INTEGRATION_50ms = 0x8,
    ALS_INTEGRATION_100ms = 0x0,
    ALS_INTEGRATION_200ms = 0x1,
    ALS_INTEGRATION_400ms = 0x2,
    ALS_INTEGRATION_800ms = 0x3,
  }eAlsItime_t;

  /**
   * @enum  eAlsPersist_t
   * @brief  ALS persistence protect number setting
   */
  typedef enum
  {
    ALS_PERSISTENCE_1 = 0x0,
    ALS_PERSISTENCE_2 = 0x1,
    ALS_PERSISTENCE_4 = 0x2,
    ALS_PERSISTENCE_8 = 0x3,
  }eAlsPersist_t;

  /**
   * @enum  eAlsPowerMode_t
   * @brief  Power saving mode
   */
  typedef enum
  {
    ALS_POWER_MODE_1 = 0x0,
    ALS_POWER_MODE_2 = 0x1,
    ALS_POWER_MODE_3 = 0x2,
    ALS_POWER_MODE_4 = 0x3,
  }eAlsPowerMode_t;

  /**
   * @enum  eAlsSTATUS_t
   * @brief  ALS status
   */
  typedef enum
  {
    STATUS_OK = 0,
    STATUS_ERROR = 0xff,
  }eAlsSTATUS_t;

  /**
   * @brief Constructor
   * @return None
   */
  VEML7700();

  /**
   * @fn setGain
   * @brief Set ALS gain coefficient
   * @param gain Gain selection:
   * @n     ALS_GAIN_x1, ALS_GAIN_x2, ALS_GAIN_d8, ALS_GAIN_d4
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t setGain(eAlsGain_t gain);

  /**
   * @fn getGain
   * @brief Get the current ALS gain coefficient
   * @param gain The current ALS gain coefficient:
   * @n     ALS_GAIN_x1, ALS_GAIN_x2, ALS_GAIN_d8, ALS_GAIN_d4
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getGain(eAlsGain_t& gain);

  /**
   * @fn setIntegrationTime
   * @brief Set ALS integration time
   * @param itime ALS integration time:
   * @n       ALS_INTEGRATION_25ms, ALS_INTEGRATION_50ms, ALS_INTEGRATION_100ms, 
   * @n       ALS_INTEGRATION_200ms, ALS_INTEGRATION_400ms, ALS_INTEGRATION_800ms
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t setIntegrationTime(eAlsItime_t itime);

  /**
   * @fn getIntegrationTime
   * @brief Get ALS integration time
   * @param itime The current ALS integration time:
   * @n       ALS_INTEGRATION_25ms, ALS_INTEGRATION_50ms, ALS_INTEGRATION_100ms, 
   * @n       ALS_INTEGRATION_200ms, ALS_INTEGRATION_400ms, ALS_INTEGRATION_800ms
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getIntegrationTime(eAlsItime_t& itime);

  /**
   * @fn setPersistence
   * @brief persistence protect number setting
   * @param persist persistence protect number:
   * @n       ALS_PERSISTENCE_1, ALS_PERSISTENCE_2, ALS_PERSISTENCE_4, ALS_PERSISTENCE_8
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t setPersistence(eAlsPersist_t persist);

  /**
   * @fn setPowerSavingMode
   * @brief Power saving mode setting
   * @param powerMode Power saving mode:
   * @n       ALS_POWER_MODE_1, ALS_POWER_MODE_2, ALS_POWER_MODE_3, ALS_POWER_MODE_4
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t setPowerSavingMode(eAlsPowerMode_t powerMode);

  /**
   * @fn setPowerSaving
   * @brief Power saving mode enable
   * @param enabled :
   * @n       0 Disable
   * @n       1 Enable
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t setPowerSaving(uint8_t enabled);

  /**
   * @fn setInterrupts
   * @brief ALS interrupt enable setting
   * @param enabled :
   * @n       0 Disable
   * @n       1 Enable
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t setInterrupts(uint8_t enabled);

  /**
   * @fn setPower
   * @brief ALS shut down setting
   * @param on :
   * @n       0 power on
   * @n       1 shut down
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t setPower(uint8_t on);

  /**
   * @fn setALSHighThreshold
   * @brief Set high threshold of interrupt
   * @param thresh 16-bit high threshold set value
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t setALSHighThreshold(uint32_t thresh);

  /**
   * @fn setALSLowThreshold
   * @brief Set low threshold of interrupt
   * @param thresh 16-bit low threshold set value
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t setALSLowThreshold(uint32_t thresh);

  /**
   * @fn getALS
   * @brief get high resolution ALS output
   * @param als Buffer the sensor data, 16-bit data
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getALS(uint32_t& als);

  /**
   * @fn getWhite
   * @brief get WHITE output
   * @param white Buffer the sensor data, 16-bit data
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getWhite(uint32_t& white);

  /**
   * @fn getHighThresholdEvent
   * @brief Get high threshold trigger interrupt event
   * @param event :
   * @n       0 Not detect high threshold interrupt event
   * @n       1 Detected high threshold interrupt event
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getHighThresholdEvent(uint8_t& event);

  /**
   * @fn getLowThresholdEvent
   * @brief Get low threshold trigger interrupt event
   * @param event :
   * @n       0 Not detect low threshold interrupt event
   * @n       1 Detected low threshold interrupt event
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getLowThresholdEvent(uint8_t& event);

  /**
   * @fn begin
   * @brief Init function
   * @return None
   */
  void begin();

  /**
   * @fn begin
   * @brief Init function
   * @param als_gain Set ALS gain coefficient
   * @return None
   */
  void begin(uint8_t als_gain);

  /**
   * @fn getALSLux
   * @brief Get measured ALS illumination intensity value
   * @param lux Buffer the sensor data, 16-bit data
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getALSLux(float& lux);

  /**
   * @fn getWhiteLux
   * @brief Get measured White illumination intensity value
   * @param lux Buffer the sensor data, 16-bit data
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getWhiteLux(float& lux);

  /**
   * @fn getAutoALSLux(float& lux)
   * @brief Get measured AutoALS illumination intensity value
   * @param lux Buffer the sensor data, 16-bit data
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getAutoALSLux(float& lux);

  /**
   * @fn getAutoWhiteLux(float& lux)
   * @brief Get measured AutoWhite illumination intensity value
   * @param lux Buffer the sensor data, 16-bit data
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getAutoWhiteLux(float& lux);

  /**
   * @fn getAutoALSLux(float& lux,
   * @n                VEML7700::eAlsGain_t& auto_gain,
   * @n                VEML7700::eAlsItime_t& auto_itime,
   * @n                uint32_t& raw_counts)
   * @brief Get measured AutoALS illumination intensity value
   * @param lux Buffer the sensor data, 16-bit data
   * @param auto_gain Gain selection
   * @param auto_itime ALS integration time setting
   * @param raw_counts Raw counts
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getAutoALSLux(float& lux,
						VEML7700::eAlsGain_t& auto_gain,
                        VEML7700::eAlsItime_t& auto_itime,
                        uint32_t& raw_counts);

  /**
   * @fn getAutoWhiteLux(float& lux,
   * @n                  VEML7700::eAlsGain_t& auto_gain,
   * @n                  VEML7700::eAlsItime_t& auto_itime,
   * @n                  uint32_t& raw_counts)
   * @brief Get measured AutoWhite illumination intensity value
   * @param lux Buffer the sensor data, 16-bit data
   * @param auto_gain Gain selection
   * @param auto_itime ALS integration time setting
   * @param raw_counts Raw counts
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getAutoWhiteLux(float& lux,
						  VEML7700::eAlsGain_t& auto_gain,
                          VEML7700::eAlsItime_t& auto_itime,
                          uint32_t& raw_counts);

  /**
   * @fn sampleDelay
   * @brief Data sampling and process delay
   */
  void sampleDelay(void);

private:
  typedef uint8_t (VEML7700::*getCountsFunction)(uint32_t& counts);
  enum { COMMAND_ALS_SM = 0x00, ALS_SM_MASK = 0x1800, ALS_SM_SHIFT = 11 };
  enum { COMMAND_ALS_IT = 0x00, ALS_IT_MASK = 0x03c0, ALS_IT_SHIFT = 6 };
  enum { COMMAND_ALS_PERS = 0x00, ALS_PERS_MASK = 0x0030, ALS_PERS_SHIFT = 4 };
  enum { COMMAND_ALS_INT_EN = 0x00, ALS_INT_EN_MASK = 0x0002,
         ALS_INT_EN_SHIFT = 1 };
  enum { COMMAND_ALS_SD = 0x00,  ALS_SD_MASK = 0x0001, ALS_SD_SHIFT = 0 };
  enum { COMMAND_ALS_WH = 0x01 };
  enum { COMMAND_ALS_WL = 0x02 };
  enum { COMMAND_PSM = 0x03, PSM_MASK = 0x0006, PSM_SHIFT = 1 };
  enum { COMMAND_PSM_EN = 0x03, PSM_EN_MASK = 0x0001, PSM_EN_SHIFT = 0 };
  enum { COMMAND_ALS = 0x04 };
  enum { COMMAND_WHITE = 0x05 };
  enum { COMMAND_ALS_IF_L = 0x06, ALS_IF_L_MASK = 0x8000, ALS_IF_L_SHIFT = 15 };
  enum { COMMAND_ALS_IF_H = 0x06, ALS_IF_H_MASK = 0x4000, ALS_IF_H_SHIFT = 14 };

  uint32_t register_cache[4];   // Register cache

  /**
   * @fn sendData
   * @brief Send data to the sensor
   * @param command Command to be sent
   * @param data Data to be sent
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  int sendData(uint8_t command, uint32_t data = 0);

  /**
   * @fn receiveData
   * @brief Receive sensor data
   * @param command Command to be sent
   * @param data Buffer the sensor data, 16-bit data
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  int receiveData(uint8_t command, uint32_t& data);

  /**
   * @fn scaleLux
   * @brief Process data based on the current set parameters
   * @param raw_counts Raw counts
   * @param lux Buffer the sensor data, 16-bit data
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  void scaleLux(uint32_t raw_counts, float& lux);

  /**
   * @fn getAutoXLux
   * @brief Process original data according to the parameters
   * @param lux Buffer the sensor data, 16-bit data
   * @param counts_func Callback function
   * @param auto_gain Gain selection
   * @param auto_itime ALS integration time setting
   * @param raw_counts Raw counts
   * @return Return operation result
   * @retval STATUS_OK Operation succeed, no anomaly
   * @retval STATUS_ERROR Operation failed
   */
  uint8_t getAutoXLux(float& lux,
					  VEML7700::getCountsFunction counts_func,
					  VEML7700::eAlsGain_t& auto_gain,
                      VEML7700::eAlsItime_t& auto_itime,
                      uint32_t& raw_counts);

};

#endif
