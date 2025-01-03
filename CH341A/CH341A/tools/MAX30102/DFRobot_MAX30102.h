/*!
 * @file DFRobot_MAX30102.h
 * @brief Define the basic structure of class DFRobot_MAX30102
 * @n This is a library used to drive heart rate and oximeter sensor
 * @n Function: freely control sensor, collect readings of red light and IR, algorithms for heart-rate and SPO2
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [YeHangYu](hangyu.ye@dfrobot.com)
 * @version  V1.0
 * @date  2020-05-30
 * @https://github.com/DFRobot/DFRobot_MAX30102
 */
#ifndef _DFROBOT_MAX30102_H
#define _DFROBOT_MAX30102_H

#include <SPO2/algorithm.h>
#include <PBA/heartRate.h>

#define MAX30102_IIC_ADDRESS  0x57 //I2C Address

//Status Registers
#define MAX30102_INTSTAT1        0x00//Interrupt Status1
#define MAX30102_INTSTAT2        0x01//Interrupt Status2
#define MAX30102_INTENABLE1      0x02//Interrupt Enable1
#define MAX30102_INTENABLE2      0x03//Interrupt Enable2
//FIFO Registers
#define MAX30102_FIFOWRITEPTR    0x04//FIFO Write Pointer, The FIFO Write Pointer points to the location where the MAX30102 writes the next sample. This pointer advances for each sample pushed on to the FIFO. It can also be changed through the I2C interface when MODE[2:0] is 010, 011, or 111.
#define MAX30102_FIFOOVERFLOW    0x05//FIFO Overflow Counter, When the FIFO is full, samples are not pushed on to the FIFO, samples are lost. OVF_COUNTER counts the number of samples lost. It saturates at 0x1F. When a complete sample is “popped” (i.e., removal of old FIFO data and shifting the samples down) from the FIFO (when the read pointer advances), OVF_COUNTER is reset to zero.
#define MAX30102_FIFOREADPTR     0x06//FIFO Read Pointer, The FIFO Read Pointer points to the location from where the processor gets the next sample from the FIFO through the I2C interface. This advances each time a sample is popped from the FIFO. The processor can also write to this pointer after reading the samples to allow rereading samples from the FIFO if there is a data communication error.
#define MAX30102_FIFODATA        0x07//FIFO Data Register, The circular FIFO depth is 32 and can hold up to 32 samples of data. The sample size depends on the number of LED channels (a.k.a. channels) configured as active. As each channel signal is stored as a 3-byte data signal, the FIFO width can be 3 bytes or 6 bytes in size.
//Configuration Registers
#define MAX30102_FIFOCONFIG      0x08//FIFO Configuration
#define MAX30102_MODECONFIG      0x09//Mode Configuration
#define MAX30102_PARTICLECONFIG  0x0A//SpO2 Configuration
#define MAX30102_LED1_PULSEAMP   0x0C//LED1 Pulse Amplitude
#define MAX30102_LED2_PULSEAMP   0x0D//LED2 Pulse Amplitude
#define MAX30102_LED3_PULSEAMP   0x0E//RESERVED
#define MAX30102_LED_PROX_AMP    0x10//RESERVED
#define MAX30102_MULTILEDCONFIG1 0x11//Multi-LED Mode Control Registers
#define MAX30102_MULTILEDCONFIG2 0x12//Multi-LED Mode Control Registers
//Die Temperature Registers
#define MAX30102_DIETEMPINT      0x1F//Die Temp Integer
#define MAX30102_DIETEMPFRAC     0x20//Die Temp Fraction
#define MAX30102_DIETEMPCONFIG   0x21//Die Temperature Config
#define MAX30102_PROXINTTHRESH   0x30//RESERVED
//Part ID Registers
#define MAX30102_REVISIONID      0xFE//Revision ID
#define MAX30102_PARTID          0xFF//Part ID:0x15
#define MAX30102_EXPECTED_PARTID  0x15

//Circular buffer for storing sensor readings, cannot be smaller than 2 
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
#define MAX30102_SENSE_BUF_SIZE  2
#else
#define MAX30102_SENSE_BUF_SIZE  30
#endif


class DFRobot_MAX30102
{
//Configuration Options 
//FIFO Configuration(Register address 0x08)
//sampleAverage(Table 3. Sample Averaging)
#define SAMPLEAVG_1     0
#define SAMPLEAVG_2     1
#define SAMPLEAVG_4     2
#define SAMPLEAVG_8     3
#define SAMPLEAVG_16    4
#define SAMPLEAVG_32    5

//Mode configuration(Register address 0x09)
//ledMode(Table 4. Mode Control)
#define MODE_REDONLY    2
#define MODE_RED_IR     3
#define MODE_MULTILED   7

//Particle sensing configuration(Register address 0x0A)
//adcRange(Table 5. SpO2 ADC Range Control)
#define ADCRANGE_2048   0
#define ADCRANGE_4096   1
#define ADCRANGE_8192   2
#define ADCRANGE_16384  3
//sampleRate(Table 6. SpO2 Sample Rate Control)
#define SAMPLERATE_50   0 
#define SAMPLERATE_100  1
#define SAMPLERATE_200  2
#define SAMPLERATE_400  3
#define SAMPLERATE_800  4
#define SAMPLERATE_1000 5
#define SAMPLERATE_1600 6
#define SAMPLERATE_3200 7
//pulseWidth(Table 7. LED Pulse Width Control)
#define PULSEWIDTH_69   0 
#define PULSEWIDTH_118  1
#define PULSEWIDTH_215  2
#define PULSEWIDTH_411  3

//Multi-LED Mode Control Registers(Register address 0x011)
#define SLOT_NONE       0
#define SLOT_RED_LED    1
#define SLOT_IR_LED     2

public:
  /*
    Interrupt Status(0x00–0x01) (pg 12)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |  A_FULL  | PPG_RDY  |  ALC_OVF |                    NONE                    | PWR_RDY  |
    * ------------------------------------------------------------------------------------------
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2   |     b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |                           NONE                                 |DIE_TEMP_RDY|   NONE   |
    * ------------------------------------------------------------------------------------------
  */
  /*
    Interrupt Enable(0x02–0x03) (pg 13)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |A_FULL_EN |PPG_RDY_EN|ALC_OVF_EN|                         NONE                          |
    * ------------------------------------------------------------------------------------------
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2   |       b1      |   b0  |
    * ------------------------------------------------------------------------------------------
    * |                           NONE                                 |DIE_TEMP_RDY_EN|  NONE |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   NONE:1;
    uint8_t   dieTemp:1;     // Internal Temperature Ready Flag
    uint8_t   NONE1:6;
    uint8_t   NONE2:5;
    uint8_t   ALCOverflow:1; // Ambient Light Cancellation Overflow
    uint8_t   dataReady:1;   // New FIFO Data Ready
    uint8_t   almostFull:1;  // FIFO Almost Full Flag
  } sEnable_t;
  /*
    FIFO Configuration(0x08) (pg 17)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4          | b3 |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |            SMP_AVE             |FIFO_ROLLOVER_EN|               FIFO_A_FULL            |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   almostFull:4; // FIFO Almost Full Value
    uint8_t   RollOver:1;   // FIFO Rolls on Full
    uint8_t   sampleAverag:3;  // Sample Averaging
  } sFIFO_t;

  /*
    Mode configuration(0x09) (pg 18)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |   SHDN   |   RESET  |             NONE               |              MODE               |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   ledMode:6; /*!< 010:Heart Rate mode, Red only. 011:SpO2 mode, Red and IR. 111:Multi-LED mode, Red and IR*/
    uint8_t   reset:1; /*!< 1:reset */
    uint8_t   shutDown:1; /*!< 0: wake up 1: put IC into low power mode*/
  } sMode_t;

  /*
    Particle sensing configuration(0x0A) (pg 18)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |   NONE   |     SPO2_ADC_RGE    |             SPO2_SR            |        LED_PW        |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   pulseWidth:2;
    uint8_t   sampleRate:3;
    uint8_t   adcRange:3;
  } sParticle_t;

  /*
    LED Pulse Amplitude(0x0C–0x0D) (pg 20)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |                                         LED1_PA                                        |
    * ------------------------------------------------------------------------------------------
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |                                         LED2_PA                                        |
    * ------------------------------------------------------------------------------------------
  */
  /*
    Multi-LED Mode Control Registers(0x011) (pg 21)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |   NONE   |              SLOT2             |   NONE   |             SLOT1               |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   SLOT1:4;
    uint8_t   SLOT2:4;
  } sMultiLED_t;

  /*!
   *@brief Circular buffer for storing samples 
   */
  typedef struct {
    uint32_t red[MAX30102_SENSE_BUF_SIZE];
    uint32_t IR[MAX30102_SENSE_BUF_SIZE];
    uint8_t head;
    uint8_t tail;
  } sSenseBuf_t;

public:

  /*!
   *@brief Constructor 
   */
  DFRobot_MAX30102(void);

  /*!
   *@brief Init sensor 
   *@param pWire IIC bus pointer object and construction device, can both pass or not pass parameters (Wire in default)
   *@param i2cAddr Chip IIC address (0x57 in default)
   *@return true or false
   */
  bool begin(uint8_t i2cAddr = MAX30102_IIC_ADDRESS);

  /*!
   *@brief Use macro definition to configure sensor 
   *@param ledBrightness LED brightness, default value: 0x1F（6.4mA), Range: 0~255（0=Off, 255=50mA）
   *@param sampleAverage Average multiple samples then draw once, reduce data throughput, default 4 samples average
   *@param ledMode LED mode, default to use red light and IR at the same time
   *@param sampleRate Sampling rate, default 400 samples every second
   *@param pulseWidth Pulse width: the longer the pulse width, the wider the detection range. Default to be Max range 
   *@param adcRange ADCMeasurement Range, default 4096 (nA)，15.63(pA) per LSB
   */
  void sensorConfiguration(uint8_t ledBrightness = 0x1F, uint8_t sampleAverage = SAMPLEAVG_4, \
                           uint8_t ledMode = MODE_MULTILED, uint8_t sampleRate = SAMPLERATE_400, \
                           uint8_t pulseWidth = PULSEWIDTH_411, uint8_t adcRange = ADCRANGE_4096);

  
  /*!
   *@brief get red value
   *@return Red light reading
   */
  uint32_t getRed(void);

  /*!
   *@brief Get IR value 
   *@return IR reading
   */
  uint32_t getIR(void);

  /*!
   *@brief Get module temperature in unit °C
   *@return Float temperature
   */
  float readTemperatureC();

  /*!
   *@brief  Get module temperature in unit ℉
   *@return Float temperature
   */
  float readTemperatureF();

  /*!
   *@brief Calculate heart rate and SPO2 
   *@param *SPO2                  [out]Calculated SPO2
   *@param *SPO2Valid             [out]If the calculated SPO2 is valid, the value is 1
   *@param *heartRate             [out]Calculated heart-rate
   *@param *heartRateValid        [out]If the calculated heart-rate is valid, the value is 1
   */
  void heartrateAndOxygenSaturation(
    uint32_t *irBuffer, uint32_t *redBuffer, unsigned int bufferLength,
	int32_t* SPO2,int8_t* SPO2Valid,int32_t* heartRate,int8_t* heartRateValid);

private:
  
  /*!
   *@brief Reset all configuration, threshold, and data regsiter. When reset completed, auto reset the reset bit
   */
  void softReset();

  /*!
   *@brief Enter power-saving mode. In this mode, all registers keep their values, and read and write normally, clear all interrupts to 0. 
   */
  void shutDown();

  /*!
   *@brief Wake-up mode, work normally 
   */
  void wakeUp();

  /*!
   *@brief Set LED mode 
   *@param mode Configure the mode by the macro definition with LEDMode comment 
   */
  void setLEDMode(uint8_t mode);
  
  /*!
   *@brief Set ADC measurement range, default 4096 (nA)，15.63(pA) per LSB
   *@param adcRange Configure ADC measuring range by the macro definition with adcRange comment
   */
  void setADCRange(uint8_t adcRange);

  /*!
   *@brief Set sampling rate 
   *@param sampleRate Configure sampling rate by the macro definition with sampleRate comment 
   */
  void setSampleRate(uint8_t sampleRate);

  /*!
   *@brief Set pulse width, the longer the pulse width, the wider the detection range 
   *@param pulseWidth Configure pulse width by the macro defintion with pulseWidth comment
   */
  void setPulseWidth(uint8_t pulseWidth);

  /*!
   *@brief Set brightness of red light 
   *@param amplitude Amplitude: 0x00 = 0mA, 0x7F = 25.4mA, 0xFF = 50mA
   */
  void setPulseAmplitudeRed(uint8_t amplitude);

  /*!
   *@brief Set IR brightness 
   *@param amplitude Amplitude: 0x00 = 0mA, 0x7F = 25.4mA, 0xFF = 50mA
   */
  void setPulseAmplitudeIR(uint8_t amplitude);

  /*!
   *@brief Configure led device according to the given number, 4 timeslot in total. We will only use slot1 and slot2. There are red light and IR light 
   *@param slotNumber Slot Number, can be 1, 2
   *@param device LED Device name：SLOT_RED_LED or SLOT_IR_LED
   */
  void enableSlot(uint8_t slotNumber, uint8_t device);

  /*!
   *@brief Disable all slots 
   */
  void disableAllSlots(void);

  /*!
   *@brief Enable FIFO Almost Full Flag. This interrupt is triggered when FIFO write pointer has a certian amount of free space.
   */
  void enableAlmostFull(void);

  /*!
   *@brief Disable FIFO Almost Full Flag
   */
  void disableAlmostFull(void);

  /*!
   *@brief Enable New FIFO Data Ready. This interrupt is triggered when there is a new sample in data FIFO
   */
  void enableDataReady(void);

  /*!
   *@brief Disable New FIFO Data Ready
   */
  void disableDataReady(void);

  /*!
   *@brief Enable Ambient Light Cancellation Overflow. This interrupt is triggered when the ambient light cancellation function of SpO2/HR photodiode reaches the maximum
   */
  void enableALCOverflow(void);

  /*!
   *@brief Disable Ambient Light Cancellation Overflow
   */
  void disableALCOverflow(void);

  /*!
   *@brief Enable Internal Temperature Ready Flag. This interrupt is triggered when the internal mold temperature conversion is done.
   */
  void enableDieTempReady(void);

  /*!
   *@brief Disable Internal Temperature Ready Flag
   */
  void disableDieTempReady(void);

  /*!
   *@brief Set sample average. The sensor will send the average of multiple samples
   *@param samples Averaged sample. Configure it by the macro definition with the comment sampleAverage 
   */
  void setFIFOAverage(uint8_t samples);

  /*!
   *@brief Enable FIFO Rolls on Full. If the FIFO is full, the FIFO address will return to 0 and FIFO will continue to fill in with new data.
   */
  void enableFIFORollover();

  /*!
   *@brief Disable FIFO Rolls on Full. New sample will be lost if FIFO is full.
   */
  void disableFIFORollover();

  /*!
   *@brief Specify the size of free space. After enabling interrupt, the "almost full" interrupt will be triggered when the set size is reached.
   *@param numberOfSamples The size of free sample space. Trigger the interrupt when free samples reached this value. 
   *@ If set to 2, the interrupt will be triggered by filling in 30 samples. If set to 0, that will be triggered in 32 samples.
   */
  void setFIFOAlmostFull(uint8_t numberOfSamples);

  /*!
   *@brief Read chip ID
   */
  uint8_t getPartID();

  /*!
   *@brief Get FIFO write pointer 
   *@return Write pointer 
   */
  uint8_t getWritePointer(void);

  /*!
   *@brief Get FIFO read pointer 
   *@return Read pointer 
   */
  uint8_t getReadPointer(void);

  /*!
   *@brief Reset FIFO
   */
  void resetFIFO(void);

  /*!
   *@brief Read new data and save it in consctructor buffer 
   */
  int getNewData(void);

  int writeReg(uint8_t reg, const void* pBuf, uint8_t size);
  int readReg(uint8_t reg, void* pBuf, uint8_t size);

private:
  uint8_t _i2cAddr;
  uint8_t _activeLEDs;
  sSenseBuf_t senseBuf;//Buffer for storing multiple groups of array
};

#endif
