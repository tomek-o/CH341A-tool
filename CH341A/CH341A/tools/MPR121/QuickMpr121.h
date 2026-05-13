/** \mainpage QuickMpr121 Documentation
 * QuickMpr121 Arduino library by somewhatlurker
 * =============================================
 * 
 * QuickMpr121 is a library for using MPR121 capacitive touch sensing ICs.
 * It's designed to be as easy to configure as possible -- changing most settings just requires setting a variable before calling start.
 * 
 * Documentation is generated for all public mpr121 members.
 *
 * For more basic information including a "quick start", check the project readme file or examples.
 * 
 * 　
 * 
 * \section notes Important notes
 * Reading data isn't thread-safe, but that shouldn't be an issue for most use cases.
 * 
 * Also note that some result buffers (returned by some functions) are shared between instances to save memory.
 * Process or save data for one mpr121 before reading data from the next (or change the MPR121_SAVE_MEMORY define to false to avoid this).
 * 
 * Changes to properties won't take effect until you restart the MPR121.
 * 
 * 
 * AN**** numbers in docs/comments refer to application notes, available on the NXP website.
 * 
 * 　
 * 
 * \section copyright Copyright
 * Copyright 2020 somewhatlurker, MIT license
 */

#ifndef QuickMpr121H
#define QuickMpr121H

#include "QuickMpr121Enums.h"
#include <stdint.h>

#define MPR121_I2CADDR_DEFAULT 0x5A

// please also update `PREDEFINED` in Doxyfile if changing any defines

#ifndef MPR121_I2C_BUFLEN
#define MPR121_I2C_BUFLEN 26 // note: arduino Wire library defines BUFFER_LENGTH as 32, so much larger values won't work
#endif

// use bitfields (stored in short) instead electrodeTouchBuf and electrodeOORBuf
#define MPR121_USE_BITFIELDS false

// make some buffers static (shared between instances) to save memory
#define MPR121_SAVE_MEMORY true


// pin names (mainly useful for LED pins)
#define MPR_ELE0 0
#define MPR_ELE1 1
#define MPR_ELE2 2
#define MPR_ELE3 3
#define MPR_ELE4 4
#define MPR_ELE5 5
#define MPR_ELE6 6
#define MPR_ELE7 7
#define MPR_ELE8 8
#define MPR_ELE9 9
#define MPR_ELE10 10
#define MPR_ELE11 11
#define MPR_ELEPROX 12

#define MPR_LED0 MPR_ELE4
#define MPR_LED1 MPR_ELE5
#define MPR_LED2 MPR_ELE6
#define MPR_LED3 MPR_ELE7
#define MPR_LED4 MPR_ELE8
#define MPR_LED5 MPR_ELE9
#define MPR_LED6 MPR_ELE10
#define MPR_LED7 MPR_ELE11


/**
 * Main mpr121 class.
 * Use one instance per MPR121.
 */
class mpr121 {
private:
  uint8_t i2cAddr; ///< I2C address from constructor

  static uint8_t i2cReadBuf[MPR121_I2C_BUFLEN]; ///< Intermediate buffer for raw I2C reads
  
  #if MPR121_SAVE_MEMORY
    static short electrodeDataBuf[13]; ///< Return buffer for analog electrode data
	static uint8_t electrodeBaselineBuf[13]; ///< Return buffer for electrode baselines
    static uint8_t electrodeCDCBuf[13]; ///< Return buffer for electrode CDC
    static mpr121FilterCDT electrodeCDTBuf[13]; ///< Return buffer for electrode CDT
    #if !MPR121_USE_BITFIELDS
      bool electrodeTouchBuf[13]; ///< Return buffer for digital electrode data
      static bool electrodeOORBuf[15]; ///< Return buffer for out-of-range flags
    #endif
  #else // MPR121_SAVE_MEMORY
    short electrodeDataBuf[13]; ///< Return buffer for analog electrode data
	uint8_t electrodeBaselineBuf[13]; ///< Return buffer for electrode baselines
    uint8_t electrodeCDCBuf[13]; ///< Return buffer for electrode CDC
    mpr121FilterCDT electrodeCDTBuf[13]; ///< Return buffer for electrode CDT
    #if !MPR121_USE_BITFIELDS
      bool electrodeTouchBuf[13]; ///< Return buffer for digital electrode data
      bool electrodeOORBuf[15]; ///< Return buffer for out-of-range flags
    #endif
  #endif // MPR121_SAVE_MEMORY
  
  #if MPR121_USE_BITFIELDS
    short electrodeTouchCache; ///< Cache for digital single electrode reads
  #endif
  unsigned long electrodeTouchCacheMicros; ///< Last update time for electrodeTouchCache (or electrodeTouchBuf if no bitfields)
  
  
  /**
   * Writes a value to an MPR121 register.
   */
  void writeRegister(mpr121Register addr, uint8_t value);

  /**
   * Reads bytes from consecutive MPR121 registers.
   * Max count is equal to the MPR121_I2C_BUFLEN define.
   */
  uint8_t* readRegister(mpr121Register addr, uint8_t count);
  
  /**
   * Reads a byte from an MPR121 register.
   */
  uint8_t readRegister(mpr121Register addr) {
    return readRegister(addr, 1)[0];
  }


  /**
   * Checks if an electrode number and count are valid and suitable for use.
   * Returns true if they can be used or false if the caller should immediately return.
   * This function may modify electrode and/or count to keep them in bounds as necessary
   */
  bool checkElectrodeNum(uint8_t &electrode, uint8_t &count);

  /**
   * Checks if an electrode number is valid and suitable for use.
   * Returns true if it can be used or false if the caller should immediately return.
   * This function may modify electrode to keep it in bounds as necessary
   */
  bool checkElectrodeNum(uint8_t &electrode);

  /**
   * Checks if a GPIO pin number and count are valid and suitable for use.
   * Returns true if they can be used or false if the caller should immediately return.
   * This function may modify pin and/or count to keep them in bounds as necessary
   */
  bool checkGPIOPinNum(uint8_t &pin, uint8_t &count);

  /**
   * Checks if a GPIO pin number is valid and suitable for use.
   * Returns true if it can be used or false if the caller should immediately return.
   * This function may modify pin to keep it in bounds as necessary
   */
  bool checkGPIOPinNum(uint8_t &pin);


  /**
   * Sets the touch and release thresholds for consecutive electrodes.
   * (AN3892)
   */
  void setElectrodeThresholds(uint8_t electrode, uint8_t count, uint8_t touchThreshold, uint8_t releaseThreshold);
  
  /**
   * Sets the touch and release thresholds for a single electrode.
   * (AN3892)
   */
  void setElectrodeThresholds(uint8_t electrode, uint8_t touchThreshold, uint8_t releaseThreshold) {
    setElectrodeThresholds(electrode, 1, touchThreshold, releaseThreshold);
  }


  /**
   * Sets the "Max Half Delta" baseline filter values.
   * Max: 63
   * (AN3891)
   */
  void setMHD(uint8_t rising, uint8_t falling);
  
  /**
   * Sets the "Noise Half Delta" baseline filter values.
   * Max: 63
   * (AN3891)
   */
  void setNHD(uint8_t rising, uint8_t falling, uint8_t touched);
  
  /**
   * Sets the "Noise Count Limit" baseline filter values.
   * (AN3891)
   */
  void setNCL(uint8_t rising, uint8_t falling, uint8_t touched);
  
  /**
   * Sets the "Filter Delay Limit" baseline filter values.
   * (AN3891)
   */
  void setFDL(uint8_t rising, uint8_t falling, uint8_t touched);


  /**
   * Sets the "Max Half Delta" baseline filter values for proximity detection.
   * Max: 63
   * (AN3891/AN3893)
   */
  void setMHDProx(uint8_t rising, uint8_t falling);
  
  /**
   * Sets the "Noise Half Delta" baseline filter values for proximity detection.
   * Max: 63
   * (AN3891/AN3893)
   */
  void setNHDProx(uint8_t rising, uint8_t falling, uint8_t touched);
  
  /**
   * Sets the "Noise Count Limit" baseline filter values for proximity detection.
   * (AN3891/AN3893)
   */
  void setNCLProx(uint8_t rising, uint8_t falling, uint8_t touched);
  
  /**
   * Sets the "Filter Delay Limit" baseline filter values for proximity detection.
   * (AN3891/AN3893)
   */
  void setFDLProx(uint8_t rising, uint8_t falling, uint8_t touched);

  
  /**
   * Sets the debounce counts.
   * A detection must be held for this many readings before the status register is updated.
   * Max: 7
   */
  void setDebounce(uint8_t touchNumber, uint8_t releaseNumber);


  /**
   * Sets "Filter Configuration".
   * (AN3890)
   * 
   * \param FFI   "First Filter Iterations"
   * \param CDC   Global "Charge Discharge Current" (μA), not used if autoconfig is enabled (Max 63, Default 16)
   * \param CDT   Global "Charge Discharge Time" (μs), not used if autoconfig is enabled
   * \param SFI   "Second Filter Iterations"
   * \param ESI   "Electrode Sample Interval" (ms)
   * 
   * Response time is equal to SFI * ESI in ms.
   */
  void setFilterConfig(mpr121FilterFFI FFI, uint8_t CDC, mpr121FilterCDT CDT, mpr121FilterSFI SFI, mpr121FilterESI ESI);

  
  /**
   * Sets "Electrode Configuration".
   * 
   * \param CL          Calibration lock (baseline tracking and initial value settings)
   * \param ELEPROX_EN  Run with these electrodes used for proximity detection
   * \param ELE_EN      Run with this number of electrodes to scan, starting from ELE0 (excludes ELEPROX)
   */
  void setElectrodeConfiguration(mpr121ElectrodeConfigCL CL, mpr121ElectrodeConfigProx ELEPROX_EN, uint8_t ELE_EN);

  
  /**
   * Sets "Auto-Configure" settings.
   * (AN3889/datasheet)
   * 
   * \param USL   "Up-Side Limit" -- Calculate this as `256L * (supplyMillivolts - 700) / supplyMillivolts`. If unsure, use the value for 1.8V supply (156).
   * \param LSL   "Low-Side Limit" -- Calculate this as `USL * 0.65`. If unsure, use the value for 1.8V supply (101).
   * \param TL    "Target Level" -- Calculate this as `USL * 0.9`. If unsure, use the value for 1.8V supply (140).
   * \param RETRY Number of retries for failed config before setting the out of range flag.
   * \param BVA   "Baseline Value Adjust" -- Changes how the baseline registers will be set after auto-configuration completes
   * \param ARE   "Automatic Reconfiguration Enable" -- Out of range (failed) channels will be reconfigured every sampling interval
   * \param ACE   "Automatic Configuration Enable" -- Perform auto-configuration when entering run mode
   * \param SCTS  "Skip Charge Time Search" -- Skip CDT search and use the already-set CDTx or global CDT. This results in a shorter time to configure, but the designer must supply appropriate values.
   * \param OORIE "Out-of-range interrupt enable" -- Trigger an interrupt when a channel is determined to be out of range
   * \param ARFIE "Auto-reconfiguration fail interrupt enable" -- Trigger an interrupt when auto-reconfiguration fails
   * \param ACFIE "Auto-configuration fail interrupt enable" -- Trigger an interrupt when auto-configuration fails
   */
  void setAutoConfig(uint8_t USL, uint8_t LSL, uint8_t TL, mpr121AutoConfigRetry RETRY, mpr121AutoConfigBVA BVA, bool ARE, bool ACE, bool SCTS, bool OORIE, bool ARFIE, bool ACFIE);


  /**
   * Sets the GPIO PWM value for consecutive pins.
   * (AN3894)
   * 
   * Max value is 15
   * Pin 9 apparently has a logic bug and pin 10 must also have its data set high for it to work.
   *   (https://community.nxp.com/thread/305474)
   */
  void setPWM(uint8_t pin, uint8_t count, uint8_t value);
  
public:
  /**
   * Creates an MPR121 device with sane default settings.
   * 

   * \param addr  The I2C address to use.
   * \param wire  You can pass in an alternative TwoWire instance.
   */
  mpr121(void);

  void setAddress(uint8_t addr) {
  	i2cAddr = addr;
  }

  uint8_t touchThresholds[13]; ///< Touch detection thresholds for ELE0-ELE11 and ELEPROX
  uint8_t releaseThresholds[13]; ///< Release detection thresholds for ELE0-ELE11 and ELEPROX

  uint8_t MHDrising; ///< "Max Half Delta" rising baseline adjustment value (AN3891) -- max: 63
  uint8_t MHDfalling; ///< "Max Half Delta" falling baseline adjustment value (AN3891) -- max: 63
  
  uint8_t NHDrising; ///< "Noise Half Delta" rising baseline adjustment value (AN3891) -- max: 63
  uint8_t NHDfalling; ///< "Noise Half Delta" falling baseline adjustment value (AN3891) -- max: 63
  uint8_t NHDtouched; ///< "Noise Half Delta" touched baseline adjustment value (AN3891) -- max: 63
  
  uint8_t NCLrising; ///< "Noise Count Limit" rising baseline adjustment value (AN3891)
  uint8_t NCLfalling; ///< "Noise Count Limit" falling baseline adjustment value (AN3891)
  uint8_t NCLtouched; ///< "Noise Count Limit" touched baseline adjustment value (AN3891)
  
  uint8_t FDLrising; ///< "Filter Delay Limit" rising baseline adjustment value (AN3891)
  uint8_t FDLfalling; ///< "Filter Delay Limit" falling baseline adjustment value (AN3891)
  uint8_t FDLtouched; ///< "Filter Delay Limit" touched baseline adjustment value (AN3891)

  
  uint8_t MHDrisingProx; ///< "Max Half Delta" rising value for proximity detection (AN3891/AN3893) -- max: 63
  uint8_t MHDfallingProx; ///< "Max Half Delta" falling value for proximity detection (AN3891/AN3893) -- max: 63
  
  uint8_t NHDrisingProx; ///< "Noise Half Delta" rising value for proximity detection (AN3891/AN3893) -- max: 63
  uint8_t NHDfallingProx; ///< "Noise Half Delta" falling value for proximity detection (AN3891/AN3893) -- max: 63
  uint8_t NHDtouchedProx; ///< "Noise Half Delta" touched value for proximity detection (AN3891/AN3893) -- max: 63
  
  uint8_t NCLrisingProx; ///< "Noise Count Limit" rising value for proximity detection (AN3891/AN3893)
  uint8_t NCLfallingProx; ///< "Noise Count Limit" falling value for proximity detection (AN3891/AN3893)
  uint8_t NCLtouchedProx; ///< "Noise Count Limit" touched value for proximity detection (AN3891/AN3893)
  
  uint8_t FDLrisingProx; ///< "Filter Delay Limit" rising value for proximity detection (AN3891/AN3893)
  uint8_t FDLfallingProx; ///< "Filter Delay Limit" falling value for proximity detection (AN3891/AN3893)
  uint8_t FDLtouchedProx; ///< "Filter Delay Limit" touched value for proximity detection (AN3891/AN3893)


  uint8_t debounceTouch; ///< Set "Debounce" count for touches (times a detection must be sampled) -- max: 7
  uint8_t debounceRelease; ///< Set "Debounce" count for releases (times a detection must be sampled) -- max: 7


  mpr121FilterFFI FFI; ///< "First Filter Iterations" (number of samples taken for the first level of filtering)
  uint8_t globalCDC; ///< Global "Charge Discharge Current" (μA), not used if autoconfig is enabled -- max 63
  mpr121FilterCDT globalCDT; ///< Global "Charge Discharge Time" (μs), not used if autoconfig is enabled
  mpr121FilterSFI SFI; ///< "Second Filter Iterations" (number of samples taken for the second level of filtering)
  mpr121FilterESI ESI; ///< "Electrode Sample Interval" (ms)


  mpr121ElectrodeConfigCL calLock; ///< "Calibration Lock" (baseline tracking and initial value settings)
  mpr121ElectrodeConfigProx proxEnable; ///< ELEPROX_EN: sets what electrodes will be used for proximity detection

  uint8_t autoConfigUSL; ///< "Up-Side Limit" for auto calibration -- if not set when starting, this will be automatically set to the ideal value for 1.8V supply
  uint8_t autoConfigLSL; ///< "Low-Side Limit" for auto calibration -- if not set when starting, this will be automatically set based on USL
  uint8_t autoConfigTL; ///< "Target Level" for auto calibration -- if not set when starting, this will be automatically set based on USL
  mpr121AutoConfigRetry autoConfigRetry; ///< Number of retries for failed auto-config before out of range will be set
  mpr121AutoConfigBVA autoConfigBaselineAdjust; ///< "Baseline Value Adjust" changes how the baseline registers will be set after auto-configuration completes
  bool autoConfigEnableReconfig; ///< "Automatic Reconfiguration Enable" will try to reconfigure out of range (failed) channels every sampling interval
  bool autoConfigEnableCalibration; ///< "Automatic Configuration Enable" will enable/disable auto-configuration when entering run mode
  
  bool autoConfigSkipChargeTime; ///< "Skip Charge Time Search" will skip searching for charge time and use the already-set per-electrode or global value
                                 ///< 
                                 ///< This results in a shorter time to configure, but the designer must supply appropriate values.
  bool autoConfigInterruptOOR; ///< "Out-of-range interrupt enable" will trigger an interrupt when a channel is determined to be out of range
  bool autoConfigInterruptARF; ///< "Auto-reconfiguration fail interrupt enable" will trigger an interrupt when auto-reconfiguration fails
  bool autoConfigInterruptACF; ///< "Auto-configuration fail interrupt enable" will trigger an interrupt when auto-configuration fails


  /** 
   * Reads one touch state bool.
   * Also use this for reading GPIO inputs.
   * 
   * When multiple touches must be read, using the variant that returns data for all 13 is preferred.
   * This does implement a simple cache though, so performance shouldn't be drastically different.
   */
  bool readTouchState(uint8_t electrode);

  #if MPR121_USE_BITFIELDS
    /** 
     * Reads the 13 touch state bits.
     * Also use this for reading GPIO inputs.
     */
    short readTouchState();

    /** Reads the 15 out of range bits.
     * [13]: auto-config fail flag
     * [14]: auto-reconfig fail flag
     */
    short readOORState();
  #else
    /** 
     * Reads the 13 touch state bools.
     * Also use this for reading GPIO inputs.
     */
    bool* readTouchState();

    /** Reads the 15 out of range bools.
     * [13]: auto-config fail flag
     * [14]: auto-reconfig fail flag
     */
    bool* readOORState();
  #endif

  /**
   * Reads the over current flag.
   * (over current on REXT pin, probably shouldn't happen in normal operation)
   */
  bool readOverCurrent();

  /**
   * Clears the over current flag.
   * (over current on REXT pin, probably shouldn't happen in normal operation)
   */
  void clearOverCurrent();

  /**
   * Reads filtered analog data for consecutive electrodes.
   */
  short* readElectrodeData(uint8_t electrode, uint8_t count);
  
  /**
   * Reads filtered analog data for a single electrode.
   */
  short readElectrodeData(uint8_t electrode) {
    return readElectrodeData(electrode, 1)[0];
  }
  
  /**
   * Reads baseline values for consecutive electrodes.
   */
  uint8_t* readElectrodeBaseline(uint8_t electrode, uint8_t count);
  
  /**
   * Reads the baseline value for a single electrode.
   */
  uint8_t readElectrodeBaseline(uint8_t electrode) {
    return readElectrodeBaseline(electrode, 1)[0];
  }

  /**
   * Writes a baseline value to consecutive electrodes.
   */
  void writeElectrodeBaseline(uint8_t electrode, uint8_t count, uint8_t value);
  
  /**
   * Writes the baseline value for a single electrode.
   */
  void writeElectrodeBaseline(uint8_t electrode, uint8_t value) {
    writeElectrodeBaseline(electrode, 1, value);
  }

  /**
   * A quick way to set all ::touchThresholds and ::releaseThresholds.
   * 
   * \param prox  Whether to set proximity detection thresholds too.
   */
  void setAllThresholds(uint8_t touched, uint8_t released, bool prox);
  
  
  /**
   * Reads per-electrode "Charge Discharge Current" (μA) for consecutive electrodes.
   */
  uint8_t* readElectrodeCDC(uint8_t electrode, uint8_t count);
  
  /**
   * Reads per-electrode "Charge Discharge Current" (μA) for a single electrode.
   */
  uint8_t readElectrodeCDC(uint8_t electrode) {
    return readElectrodeCDC(electrode, 1)[0];
  }
  
  /**
   * Writes per-electrode "Charge Discharge Current" (μA) for consecutive electrodes.
   * Max: 63
   */
  void writeElectrodeCDC(uint8_t electrode, uint8_t count, uint8_t value);
  
  /**
   * Writes per-electrode "Charge Discharge Current" (μA) for a single electrode.
   * Max: 63
   */
  void writeElectrodeCDC(uint8_t electrode, uint8_t value) {
    writeElectrodeCDC(electrode, 1, value);
  }
  
  /**
   * Reads per-electrode "Charge Discharge Time" (μs) for consecutive electrodes.
   */
  mpr121FilterCDT* readElectrodeCDT(uint8_t electrode, uint8_t count);
  
  /**
   * Reads per-electrode "Charge Discharge Time" (μs) for a single electrode.
   */
  mpr121FilterCDT readElectrodeCDT(uint8_t electrode) {
    return readElectrodeCDT(electrode, 1)[0];
  }
  
  /**
   * Writes per-electrode "Charge Discharge Time" (μs) for consecutive electrodes.
   */
  void writeElectrodeCDT(uint8_t electrode, uint8_t count, mpr121FilterCDT value);
  
  /**
   * Writes per-electrode "Charge Discharge Time" (μs) for a single electrode.
   */
  void writeElectrodeCDT(uint8_t electrode, mpr121FilterCDT value) {
    writeElectrodeCDT(electrode, 1, value);
  }


  /**
   * Sets pin mode for consecutive GPIO pins.
   * 
   * GPIO can be used on pins 4-11 when they aren't used for sensing.
   * Use mode MPR_GPIO_MODE_OUTPUT_OPENDRAIN_HIGH for direct LED driving -- it can source up to 12mA.
   */
  void setGPIOMode(uint8_t pin, uint8_t count, mpr121GPIOMode mode);

  /**
   * Sets pin mode for a single GPIO pin.
   * 
   * GPIO can be used on pins 4-11 when they aren't used for sensing.
   * Use mode MPR_GPIO_MODE_OUTPUT_OPENDRAIN_HIGH for direct LED driving -- it can source up to 12mA.
   */
  void setGPIOMode(uint8_t pin, mpr121GPIOMode mode) {
    setGPIOMode(pin, 1, mode);
  }

  /**
   * Writes a digital value to consecutive GPIO pins.
   */
  void writeGPIODigital(uint8_t pin, uint8_t count, bool value);

  /**
   * Writes a digital value to a single GPIO pin.
   */
  void writeGPIODigital(uint8_t pin, bool value) {
    writeGPIODigital(pin, 1, value);
  }

  /**
   * Writes an "analog" (PWM) value to consecutive GPIO pins.
   * Max value is 15
   * 
   * Pin 9 apparently has a logic bug and pin 10 must also have its data set high for it to work.
   *   (see https://community.nxp.com/thread/305474)
   */
  void writeGPIOAnalog(uint8_t pin, uint8_t count, uint8_t value);

  /**
   * Writes an "analog" (PWM) value to a single GPIO pin.
   * Max value is 15
   * 
   * Pin 9 apparently has a logic bug and pin 10 must also have its data set high for it to work.
   *   (see https://community.nxp.com/thread/305474)
   */
  void writeGPIOAnalog(uint8_t pin, uint8_t value) {
    writeGPIOAnalog(pin, 1, value);
  }


  /**
   * Applies settings and enters run mode with a given number of electrodes.
   * Very much based on the quick start guide (AN3944).
   */
  void start(uint8_t electrodes);


  /**
   * Exits run mode.
   */
  void stop();



  /**
   * Checks if the MPR121 is in run mode.
   */
  bool checkRunning();


  /**
   * Resets the MPR121.
   */
  void softReset();
};

#endif

