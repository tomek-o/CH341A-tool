/*!
 * @file DFRobot_MAX30102.h
 * @brief Define the basic structure of class DFRobot_MAX30102
 * @n This is a library used to drive heart rate and oximeter sensor
 * @n Function: freely control sensor, collect red light and IR readings, algorithms for heart-rate and SPO2
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [YeHangYu](hangyu.ye@dfrobot.com)
 * @version  V1.0
 * @date  2020-05-30
 * @https://github.com/DFRobot/DFRobot_MAX30102
 */
#include <vcl.h>
#pragma hdrstop

#include <DFRobot_MAX30102.h>
#include "CH341A.h"
#include "Log.h"
#include <windows.h>

#ifdef __BORLANDC__
#pragma warn -8071
#endif

#define ENABLE_DBG
#ifdef ENABLE_DBG
#define DBG(...) {LOG(__VA_ARGS__);}
#else
#define DBG(...)
#endif

namespace
{

uint32_t millis(void)
{
	return static_cast<uint32_t>(GetTickCount());
}

void delay(uint32_t ms)
{
	Sleep(ms);
	Application->ProcessMessages();
}

}

DFRobot_MAX30102::DFRobot_MAX30102(void)
{

}

bool DFRobot_MAX30102::begin(uint8_t i2cAddr)
{
  _i2cAddr = i2cAddr;

  if (getPartID() != MAX30102_EXPECTED_PARTID) {
    DBG("not expected partid\n");
    return false;
  }
  softReset();
  return true;
}

void DFRobot_MAX30102::enableAlmostFull(void)
{
  sEnable_t enableReg;
  readReg(MAX30102_INTENABLE1, &enableReg, 2);
  enableReg.almostFull = 1;
  writeReg(MAX30102_INTENABLE1, &enableReg, 2);
}
void DFRobot_MAX30102::disableAlmostFull(void)
{
  sEnable_t enableReg;
  readReg(MAX30102_INTENABLE1, &enableReg, 2);
  enableReg.almostFull = 0;
  writeReg(MAX30102_INTENABLE1, &enableReg, 2);
}

void DFRobot_MAX30102::enableDataReady(void)
{
  sEnable_t enableReg;
  readReg(MAX30102_INTENABLE1, &enableReg, 2);
  enableReg.dataReady = 1;
  writeReg(MAX30102_INTENABLE1, &enableReg, 2);
}
void DFRobot_MAX30102::disableDataReady(void)
{
  sEnable_t enableReg;
  readReg(MAX30102_INTENABLE1, &enableReg, 2);
  enableReg.dataReady = 0;
  writeReg(MAX30102_INTENABLE1, &enableReg, 2);
}

void DFRobot_MAX30102::enableALCOverflow(void)
{
  sEnable_t enableReg;
  readReg(MAX30102_INTENABLE1, &enableReg, 2);
  enableReg.ALCOverflow = 1;
  writeReg(MAX30102_INTENABLE1, &enableReg, 2);
}
void DFRobot_MAX30102::disableALCOverflow(void)
{
  sEnable_t enableReg;
  readReg(MAX30102_INTENABLE1, &enableReg, 2);
  enableReg.ALCOverflow = 0;
  writeReg(MAX30102_INTENABLE1, &enableReg, 2);
}

void DFRobot_MAX30102::enableDieTempReady(void)
{
  sEnable_t enableReg;
  readReg(MAX30102_INTENABLE1, &enableReg, 2);
  enableReg.dieTemp = 1;
  writeReg(MAX30102_INTENABLE1, &enableReg, 2);
}
void DFRobot_MAX30102::disableDieTempReady(void)
{
  sEnable_t enableReg;
  readReg(MAX30102_INTENABLE1, &enableReg, 2);
  enableReg.dieTemp = 0;
  writeReg(MAX30102_INTENABLE1, &enableReg, 2);
}

void DFRobot_MAX30102::softReset(void)
{
  sMode_t modeReg;
  readReg(MAX30102_MODECONFIG, &modeReg, 1);
  modeReg.reset = 1;
  writeReg(MAX30102_MODECONFIG, &modeReg, 1);
  uint32_t startTime = millis();
  while (millis() - startTime < 100) {
    readReg(MAX30102_MODECONFIG, &modeReg, 1);
    if (modeReg.reset == 0) break; 
    delay(1);
  }
}

void DFRobot_MAX30102::shutDown(void)
{
  sMode_t modeReg;
  readReg(MAX30102_MODECONFIG, &modeReg, 1);
  modeReg.shutDown = 1;
  writeReg(MAX30102_MODECONFIG, &modeReg, 1);
}

void DFRobot_MAX30102::wakeUp(void)
{
  sMode_t modeReg;
  readReg(MAX30102_MODECONFIG, &modeReg, 1);
  modeReg.shutDown = 0;
  writeReg(MAX30102_MODECONFIG, &modeReg, 1);
}

void DFRobot_MAX30102::setLEDMode(uint8_t ledMode)
{
  sMode_t modeReg;
  readReg(MAX30102_MODECONFIG, &modeReg, 1);
  modeReg.ledMode = ledMode;
  writeReg(MAX30102_MODECONFIG, &modeReg, 1);
}

void DFRobot_MAX30102::setADCRange(uint8_t adcRange)
{
  sParticle_t particleReg;
  readReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
  particleReg.adcRange = adcRange;
  writeReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
}

void DFRobot_MAX30102::setSampleRate(uint8_t sampleRate)
{
  sParticle_t particleReg;
  readReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
  particleReg.sampleRate = sampleRate;
  writeReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
}

void DFRobot_MAX30102::setPulseWidth(uint8_t pulseWidth)
{
  sParticle_t particleReg;
  readReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
  particleReg.pulseWidth = pulseWidth;
  writeReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
}

void DFRobot_MAX30102::setPulseAmplitudeRed(uint8_t amplitude)
{
  uint8_t byteTemp = amplitude;
  writeReg(MAX30102_LED1_PULSEAMP, &byteTemp, 1);
}

void DFRobot_MAX30102::setPulseAmplitudeIR(uint8_t amplitude)
{
  uint8_t byteTemp = amplitude;
  writeReg(MAX30102_LED2_PULSEAMP, &byteTemp, 1);
}

void DFRobot_MAX30102::enableSlot(uint8_t slotNumber, uint8_t device)
{
  sMultiLED_t multiLEDReg;
  switch (slotNumber) {
  case (1):
    readReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
    multiLEDReg.SLOT1 = device;
    writeReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
    break;
  case (2):
    readReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
    multiLEDReg.SLOT2 = device;
    writeReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
    break;
  default:
    break;
  }
}

void DFRobot_MAX30102::disableAllSlots(void)
{
  sMultiLED_t multiLEDReg;
  multiLEDReg.SLOT1 = 0;
  multiLEDReg.SLOT2 = 0;
  writeReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
}

void DFRobot_MAX30102::resetFIFO(void)
{
  uint8_t byteTemp = 0;
  writeReg(MAX30102_FIFOWRITEPTR, &byteTemp, 1);
  writeReg(MAX30102_FIFOOVERFLOW, &byteTemp, 1);
  writeReg(MAX30102_FIFOREADPTR, &byteTemp, 1);
}

uint8_t DFRobot_MAX30102::getWritePointer(void)
{
  uint8_t byteTemp;
  readReg(MAX30102_FIFOWRITEPTR, &byteTemp, 1);
  return byteTemp;
}

uint8_t DFRobot_MAX30102::getReadPointer(void)
{
  uint8_t byteTemp;
  readReg(MAX30102_FIFOREADPTR, &byteTemp, 1);
  return byteTemp;
}



void DFRobot_MAX30102::setFIFOAverage(uint8_t numberOfSamples)
{
  sFIFO_t FIFOReg;
  readReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
  FIFOReg.sampleAverag = numberOfSamples;
  writeReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
}

void DFRobot_MAX30102::enableFIFORollover(void)
{
  sFIFO_t FIFOReg;
  readReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
  FIFOReg.RollOver = 1;
  writeReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
}

void DFRobot_MAX30102::disableFIFORollover(void)
{
  sFIFO_t FIFOReg;
  readReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
  FIFOReg.RollOver = 0;
  writeReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
}

void DFRobot_MAX30102::setFIFOAlmostFull(uint8_t numberOfSamples)
{
  sFIFO_t FIFOReg;
  readReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
  FIFOReg.almostFull = numberOfSamples;
  writeReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
}



float DFRobot_MAX30102::readTemperatureC()
{

  uint8_t byteTemp = 0x01;
  writeReg(MAX30102_DIETEMPCONFIG, &byteTemp, 1);

  uint32_t startTime = millis();
  while (millis() - startTime < 100) { 
    readReg(MAX30102_DIETEMPCONFIG, &byteTemp, 1);
    if ((byteTemp & 0x01) == 0) break; 
    delay(1);
  }


  uint8_t tempInt;
  readReg(MAX30102_DIETEMPINT, &tempInt, 1);

  uint8_t tempFrac;
  readReg(MAX30102_DIETEMPFRAC, &tempFrac, 1);

  return (float)tempInt + ((float)tempFrac * 0.0625);
}

float DFRobot_MAX30102::readTemperatureF()
{
  float temp = readTemperatureC();
  if (temp != -999.0) temp = temp * 1.8 + 32.0;
  return (temp);
}

uint8_t DFRobot_MAX30102::getPartID()
{
  uint8_t byteTemp;
  readReg(MAX30102_PARTID, &byteTemp, 1);
  return byteTemp;
}


void DFRobot_MAX30102::sensorConfiguration(uint8_t ledBrightness, uint8_t sampleAverage, uint8_t ledMode, uint8_t sampleRate, uint8_t pulseWidth, uint8_t adcRange)
{

  setFIFOAverage(sampleAverage);


  setADCRange(adcRange);

  setSampleRate(sampleRate);

  setPulseWidth(pulseWidth);

  setPulseAmplitudeRed(ledBrightness);
  setPulseAmplitudeIR(ledBrightness);

  enableSlot(1, SLOT_RED_LED);
  if (ledMode > MODE_REDONLY) enableSlot(2, SLOT_IR_LED);


  setLEDMode(ledMode);

  if (ledMode == MODE_REDONLY) {
    _activeLEDs = 1;
  } else {
    _activeLEDs = 2;
  }

  enableFIFORollover(); 
  resetFIFO(); 
}


uint32_t DFRobot_MAX30102::getRed(void)
{
  getNewData();
  return (senseBuf.red[senseBuf.head]);
}

uint32_t DFRobot_MAX30102::getIR(void)
{
  getNewData();
  return (senseBuf.IR[senseBuf.head]);
}

int DFRobot_MAX30102::getNewData(void)
{
  int32_t numberOfSamples = 0;
  uint8_t readPointer = 0;
  uint8_t writePointer = 0;
  uint32_t start = millis();
  while (1) {
	readPointer = getReadPointer();
	writePointer = getWritePointer();

    if (readPointer == writePointer) {
	  //DBG("no data\n");
	  if (millis() - start > 10000)
		return -1;
    } else {
	  numberOfSamples = writePointer - readPointer;
	  if (numberOfSamples < 0) numberOfSamples += 32;
      DBG("MAX30102 has %d samples\n", numberOfSamples);	  
	  int32_t bytesNeedToRead = numberOfSamples * _activeLEDs * 3;
   
        while (bytesNeedToRead > 0) {
          senseBuf.head++;
          senseBuf.head %= MAX30102_SENSE_BUF_SIZE;
          uint32_t tempBuf = 0;
          if (_activeLEDs > 1) { 
            uint8_t temp[6];
            uint8_t tempex;

            readReg(MAX30102_FIFODATA, temp, 6);

            for(uint8_t i = 0; i < 3; i++){
              tempex = temp[i];
              temp[i] = temp[5-i];
              temp[5-i] = tempex;
            }

            memcpy(&tempBuf, temp, 3*sizeof(temp[0]));
            tempBuf &= 0x3FFFF;
            senseBuf.IR[senseBuf.head] = tempBuf;
            memcpy(&tempBuf, temp+3, 3*sizeof(temp[0]));
            tempBuf &= 0x3FFFF;
            senseBuf.red[senseBuf.head] = tempBuf;
          } else { 
            uint8_t temp[3];
            uint8_t tempex;


            readReg(MAX30102_FIFODATA, temp, 3);
            tempex = temp[0];
            temp[0] = temp[2];
            temp[2] = tempex;

            memcpy(&tempBuf, temp, 3*sizeof(temp[0]));
            tempBuf &= 0x3FFFF;
            senseBuf.red[senseBuf.head] = tempBuf;
          }
          bytesNeedToRead -= _activeLEDs * 3;
        }
      return 0;
	}
	delay(1);
  }
  return 0;
}

void DFRobot_MAX30102::heartrateAndOxygenSaturation(
    uint32_t *irBuffer, uint32_t *redBuffer, unsigned int bufferLength,
	int32_t* SPO2,int8_t* SPO2Valid,int32_t* heartRate,int8_t* heartRateValid)
{
  for (unsigned int i = 0 ; i < bufferLength ; ) {
	if (getNewData() != 0) {
		*SPO2Valid = 0;
		*SPO2 = -999;
		*heartRateValid = 0;
		*heartRate = -999;
		return;
	}
 
    int8_t numberOfSamples = senseBuf.head - senseBuf.tail;
    if (numberOfSamples < 0) {
      numberOfSamples += MAX30102_SENSE_BUF_SIZE;
    }

    while(numberOfSamples--) {
      redBuffer[i] = senseBuf.red[senseBuf.tail];
      irBuffer[i] = senseBuf.IR[senseBuf.tail];

      senseBuf.tail++;
      senseBuf.tail %= MAX30102_SENSE_BUF_SIZE;
      i++;
      if(i == bufferLength) break;
    }
  }

  maxim_heart_rate_and_oxygen_saturation(/**pun_ir_buffer=*/irBuffer, /*n_ir_buffer_length=*/bufferLength, /**pun_red_buffer=*/redBuffer, \
      /**pn_spo2=*/SPO2, /**pch_spo2_valid=*/SPO2Valid, /**pn_heart_rate=*/heartRate, /**pch_hr_valid=*/heartRateValid);
}

int DFRobot_MAX30102::writeReg(uint8_t reg, const void* pBuf, uint8_t size)
{
  if(pBuf == NULL) {
	DBG("pBuf ERROR!! : null pointer\n");
  }
  return ch341a.I2CWriteCommandWriteBytes(_i2cAddr, reg, reinterpret_cast<const uint8_t*>(pBuf), size);
}

int DFRobot_MAX30102::readReg(uint8_t reg, void* pBuf, uint8_t size)
{
  if(pBuf == NULL) {
    DBG("pBuf ERROR!! : null pointer\n");
  }
  int ret = ch341a.I2CWriteCommandReadBuffer(_i2cAddr, reg, reinterpret_cast<uint8_t*>(pBuf), size);
  if (ret != 0)
  	return -1;
  return size;
}
