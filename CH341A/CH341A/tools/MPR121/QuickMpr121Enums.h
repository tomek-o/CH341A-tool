/** \file QuickMpr121Enums.h
 * enums for QuickMpr121
 * 
 * Copyright 2020 somewhatlurker, MIT license
 */

#ifndef QuickMpr121EnumsH
#define QuickMpr121EnumsH


/// possible "first filter iterations" settings
enum mpr121FilterFFI {
  MPR_FFI_6 = 0, ///< 6 samples taken
  MPR_FFI_10 = 1, ///< 10 samples taken
  MPR_FFI_18 = 2, ///< 18 samples taken
  MPR_FFI_34 = 3, ///< 34 samples taken
};

/// possible "charge discharge time" settings
enum mpr121FilterCDT {
  MPR_CDT_DISABLED = 0, ///< disable charging
  MPR_CDT_0_5 = 1, ///< 0.5 microseconds
  MPR_CDT_1 = 2, ///< 1 microsecond
  MPR_CDT_2 = 3, ///< 2 microseconds
  MPR_CDT_4 = 4, ///< 4 microseconds
  MPR_CDT_8 = 5, ///< 8 microseconds
  MPR_CDT_16 = 6, ///< 16 microseconds
  MPR_CDT_32 = 7, ///< 32 microseconds
};

/// possible "second filter iterations" settings
enum mpr121FilterSFI {
  MPR_SFI_4 = 0, ///< 4 samples taken
  MPR_SFI_6 = 1, ///< 6 samples taken
  MPR_SFI_10 = 2, ///< 10 samples taken
  MPR_SFI_18 = 3, ///< 18 samples taken
};

/// possible "electrode sample interval" settings
enum mpr121FilterESI {
  MPR_ESI_1 = 0, ///< 1 ms
  MPR_ESI_2 = 1, ///< 2 ms
  MPR_ESI_4 = 2, ///< 4 ms
  MPR_ESI_8 = 3, ///< 8 ms
  MPR_ESI_16 = 4, ///< 16 ms
  MPR_ESI_32 = 5, ///< 32 ms
  MPR_ESI_64 = 6, ///< 64 ms
  MPR_ESI_128 = 7, ///< 128 ms
};

/// possible "calibration lock" settings
enum mpr121ElectrodeConfigCL {
  MPR_CL_TRACKING_ENABLED = 0, ///< Baseline tracking enabled, initial baseline value is current value in baseline value register
  MPR_CL_TRACKING_DISABLED = 1, ///< Baseline tracking disabled
  MPR_CL_TRACKING_ENABLED_LOAD5 = 2, ///< Baseline tracking enabled, initial baseline value is loaded with the 5 high bits of the first 10-bit electrode data value
  MPR_CL_TRACKING_ENABLED_LOAD10 = 3, ///< Baseline tracking enabled, initial baseline value is loaded with all 10 bits of the first electrode data value
};

/// possible "proximity enable" settings
enum mpr121ElectrodeConfigProx {
  MPR_ELEPROX_DISABLED = 0, ///< Proximity detection disabled
  MPR_ELEPROX_0_TO_1 = 1, ///< Run with electrodes 0 to 1 combined for proximity detection enabled.
  MPR_ELEPROX_0_TO_3 = 2, ///< Run with electrodes 0 to 3 combined for proximity detection enabled.
  MPR_ELEPROX_0_TO_11 = 3, ///< Run with electrodes 0 to 11 combined for proximity detection enabled.
};

/// possible auto-configuration "retry" settings
enum mpr121AutoConfigRetry {
  MPR_AUTOCONFIG_RETRY_DISABLED = 0, ///< No retries
  MPR_AUTOCONFIG_RETRY_2 = 1, ///< 2 retries
  MPR_AUTOCONFIG_RETRY_4 = 2, ///< 4 retries
  MPR_AUTOCONFIG_RETRY_8 = 3, ///< 8 retries
};

/// possible auto-configuration "baseline value adjust" settings
/// 
/// datasheet says this is the same as CL, but AN3899 is better here
enum mpr121AutoConfigBVA {
  MPR_AUTOCONFIG_BVA_DISABLED = 0, ///< Baseline is not changed
  MPR_AUTOCONFIG_BVA_CLEAR = 1, ///< Baseline is cleared
  MPR_AUTOCONFIG_BVA_SET_CLEAR3 = 2, ///< Baseline is set to the auto-config baseline with the lower 3 bits cleared
  MPR_AUTOCONFIG_BVA_SET_ALL = 3, ///< Baseline is set to the auto-config baseline
};

/// GPIO pin modes
enum mpr121GPIOMode {
  // bottom two bits are CTL0:CTL1
  // bit 2 is DIR
  // bit 3 is EN
  MPR_GPIO_MODE_DISABLED = 0x00, ///< Pin is disabled
  MPR_GPIO_MODE_INPUT = 0x08, ///< Input port with no pullup/pulldown
  MPR_GPIO_MODE_INPUT_PULLDOWN = 0x0A, ///< Input port with a internal pulldown
  MPR_GPIO_MODE_INPUT_PULLUP = 0x0B, ///< Input port with a internal pullup
  MPR_GPIO_MODE_OUTPUT = 0x0C, ///< CMOS output port
  MPR_GPIO_MODE_OUTPUT_OPENDRAIN_LOW = 0x0E, ///< Low side only open drain output port
  MPR_GPIO_MODE_OUTPUT_OPENDRAIN_HIGH = 0x0F, ///< High side only open drain output port for LED driver
};

/// all MPR121 registers
enum mpr121Register {
  MPRREG_ELE0_TO_ELE7_TOUCH_STATUS = 0x00,
  MPRREG_ELE8_TO_ELEPROX_TOUCH_STATUS = 0x01,
  MPRREG_ELE0_TO_ELE7_OOR_STATUS = 0x02,
  MPRREG_ELE8_TO_ELEPROX_OOR_STATUS = 0x03,
  
  MPRREG_ELE0_FILTERED_DATA_LSB = 0x04,
  MPRREG_ELE0_FILTERED_DATA_MSB = 0x05,
  MPRREG_ELE1_FILTERED_DATA_LSB = 0x06,
  MPRREG_ELE1_FILTERED_DATA_MSB = 0x07,
  MPRREG_ELE2_FILTERED_DATA_LSB = 0x08,
  MPRREG_ELE2_FILTERED_DATA_MSB = 0x09,
  MPRREG_ELE3_FILTERED_DATA_LSB = 0x0a,
  MPRREG_ELE3_FILTERED_DATA_MSB = 0x0b,
  MPRREG_ELE4_FILTERED_DATA_LSB = 0x0c,
  MPRREG_ELE4_FILTERED_DATA_MSB = 0x0d,
  MPRREG_ELE5_FILTERED_DATA_LSB = 0x0e,
  MPRREG_ELE5_FILTERED_DATA_MSB = 0x0f,
  MPRREG_ELE6_FILTERED_DATA_LSB = 0x10,
  MPRREG_ELE6_FILTERED_DATA_MSB = 0x11,
  MPRREG_ELE7_FILTERED_DATA_LSB = 0x12,
  MPRREG_ELE7_FILTERED_DATA_MSB = 0x13,
  MPRREG_ELE8_FILTERED_DATA_LSB = 0x14,
  MPRREG_ELE8_FILTERED_DATA_MSB = 0x15,
  MPRREG_ELE9_FILTERED_DATA_LSB = 0x16,
  MPRREG_ELE9_FILTERED_DATA_MSB = 0x17,
  MPRREG_ELE10_FILTERED_DATA_LSB = 0x18,
  MPRREG_ELE10_FILTERED_DATA_MSB = 0x19,
  MPRREG_ELE11_FILTERED_DATA_LSB = 0x1a,
  MPRREG_ELE11_FILTERED_DATA_MSB = 0x1b,
  MPRREG_ELEPROX_FILTERED_DATA_LSB = 0x1c,
  MPRREG_ELEPROX_FILTERED_DATA_MSB = 0x1d,

  MPRREG_ELE0_BASELINE = 0x1e,
  MPRREG_ELE1_BASELINE = 0x1f,
  MPRREG_ELE2_BASELINE = 0x20,
  MPRREG_ELE3_BASELINE = 0x21,
  MPRREG_ELE4_BASELINE = 0x22,
  MPRREG_ELE5_BASELINE = 0x23,
  MPRREG_ELE6_BASELINE = 0x24,
  MPRREG_ELE7_BASELINE = 0x25,
  MPRREG_ELE8_BASELINE = 0x26,
  MPRREG_ELE9_BASELINE = 0x27,
  MPRREG_ELE10_BASELINE = 0x28,
  MPRREG_ELE11_BASELINE = 0x29,
  MPRREG_ELEPROX_BASELINE = 0x2a,
  
  MPRREG_MHD_RISING = 0x2b,
  MPRREG_NHD_AMOUNT_RISING = 0x2c,
  MPRREG_NCL_RISING = 0x2d,
  MPRREG_FDL_RISING = 0x2e,
  MPRREG_MHD_FALLING = 0x2f,
  MPRREG_NHD_AMOUNT_FALLING = 0x30,
  MPRREG_NCL_FALLING = 0x31,
  MPRREG_FDL_FALLING = 0x32,
  MPRREG_NHD_AMOUNT_TOUCHED = 0x33,
  MPRREG_NCL_TOUCHED = 0x34,
  MPRREG_FDL_TOUCHED = 0x35,

  MPRREG_ELEPROX_MHD_RISING = 0x36,
  MPRREG_ELEPROX_NHD_AMOUNT_RISING = 0x37,
  MPRREG_ELEPROX_NCL_RISING = 0x38,
  MPRREG_ELEPROX_FDL_RISING = 0x39,
  MPRREG_ELEPROX_MHD_FALLING = 0x3a,
  MPRREG_ELEPROX_NHD_AMOUNT_FALLING = 0x3b,
  MPRREG_ELEPROX_NCL_FALLING = 0x3c,
  MPRREG_ELEPROX_FDL_FALLING = 0x3d,
  MPRREG_ELEPROX_NHD_AMOUNT_TOUCHED = 0x3e,
  MPRREG_ELEPROX_NCL_TOUCHED = 0x3f,
  MPRREG_ELEPROX_FDL_TOUCHED = 0x40,
  
  MPRREG_ELE0_TOUCH_THRESHOLD = 0x41,
  MPRREG_ELE0_RELEASE_THRESHOLD = 0x42,
  MPRREG_ELE1_TOUCH_THRESHOLD = 0x43,
  MPRREG_ELE1_RELEASE_THRESHOLD = 0x44,
  MPRREG_ELE2_TOUCH_THRESHOLD = 0x45,
  MPRREG_ELE2_RELEASE_THRESHOLD = 0x46,
  MPRREG_ELE3_TOUCH_THRESHOLD = 0x47,
  MPRREG_ELE3_RELEASE_THRESHOLD = 0x48,
  MPRREG_ELE4_TOUCH_THRESHOLD = 0x49,
  MPRREG_ELE4_RELEASE_THRESHOLD = 0x4a,
  MPRREG_ELE5_TOUCH_THRESHOLD = 0x4b,
  MPRREG_ELE5_RELEASE_THRESHOLD = 0x4c,
  MPRREG_ELE6_TOUCH_THRESHOLD = 0x4d,
  MPRREG_ELE6_RELEASE_THRESHOLD = 0x4e,
  MPRREG_ELE7_TOUCH_THRESHOLD = 0x4f,
  MPRREG_ELE7_RELEASE_THRESHOLD = 0x50,
  MPRREG_ELE8_TOUCH_THRESHOLD = 0x51,
  MPRREG_ELE8_RELEASE_THRESHOLD = 0x52,
  MPRREG_ELE9_TOUCH_THRESHOLD = 0x53,
  MPRREG_ELE9_RELEASE_THRESHOLD = 0x54,
  MPRREG_ELE10_TOUCH_THRESHOLD = 0x55,
  MPRREG_ELE10_RELEASE_THRESHOLD = 0x56,
  MPRREG_ELE11_TOUCH_THRESHOLD = 0x57,
  MPRREG_ELE11_RELEASE_THRESHOLD = 0x58,
  MPRREG_ELEPROX_TOUCH_THRESHOLD = 0x59,
  MPRREG_ELEPROX_RELEASE_THRESHOLD = 0x5a,
  
  MPRREG_DEBOUNCE = 0x5b,
  
  MPRREG_FILTER_GLOBAL_CDC_CONFIG = 0x5c,
  MPRREG_AFE_CONFIG = 0x5c, // alternate name
  MPRREG_FILTER_GLOBAL_CDT_CONFIG = 0x5d,
  MPRREG_FILTER_CONFIG = 0x5d, // alternate name
  
  MPRREG_ELECTRODE_CONFIG = 0x5e,
  
  MPRREG_ELE0_CDC = 0x5f,
  MPRREG_ELE1_CDC = 0x60,
  MPRREG_ELE2_CDC = 0x61,
  MPRREG_ELE3_CDC = 0x62,
  MPRREG_ELE4_CDC = 0x63,
  MPRREG_ELE5_CDC = 0x64,
  MPRREG_ELE6_CDC = 0x65,
  MPRREG_ELE7_CDC = 0x66,
  MPRREG_ELE8_CDC = 0x67,
  MPRREG_ELE9_CDC = 0x68,
  MPRREG_ELE10_CDC = 0x69,
  MPRREG_ELE11_CDC = 0x6a,
  MPRREG_ELEPROX_CDC = 0x6b,
  
  MPRREG_ELE0_ELE1_CDT = 0x6c,
  MPRREG_ELE2_ELE3_CDT = 0x6d,
  MPRREG_ELE4_ELE5_CDT = 0x6e,
  MPRREG_ELE6_ELE7_CDT = 0x6f,
  MPRREG_ELE8_ELE9_CDT = 0x70,
  MPRREG_ELE10_ELE11_CDT = 0x71,
  MPRREG_ELEPROX_CDT = 0x72,
  
  MPRREG_GPIO_CONTROL_0 = 0x73,
  MPRREG_GPIO_CONTROL_1 = 0x74,
  MPRREG_GPIO_DATA = 0x75,
  MPRREG_GPIO_DIRECTION = 0x76,
  MPRREG_GPIO_ENABLE = 0x77,
  MPRREG_GPIO_DATA_SET = 0x78,
  MPRREG_GPIO_DATA_CLEAR = 0x79,
  MPRREG_GPIO_DATA_TOGGLE = 0x7a,
  
  MPRREG_AUTOCONFIG_CONTROL_0 = 0x7b,
  MPRREG_AUTOCONFIG_CONTROL_1 = 0x7c,
  MPRREG_AUTOCONFIG_USL = 0x7d,
  MPRREG_AUTOCONFIG_LSL = 0x7e,
  MPRREG_AUTOCONFIG_TL = 0x7f,
  
  MPRREG_SOFT_RESET = 0x80,

  MPRREG_PWM_DUTY_0 = 0x81,
  MPRREG_PWM_DUTY_1 = 0x82,
  MPRREG_PWM_DUTY_2 = 0x83,
  MPRREG_PWM_DUTY_3 = 0x84,
};


#endif
