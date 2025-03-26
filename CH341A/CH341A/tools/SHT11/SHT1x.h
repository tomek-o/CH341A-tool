/** \file SHT1x.h
	\author Tomasz Ostrowski
	\date August 2007, adapted for CH341: March 2025
	\brief Routines for Sensirion SHT1x sensor family
*/

#ifndef SHT1xH
#define SHT1xH

#include <stdint.h>

typedef union
{
    int32_t i;
    float f;
} value;


enum{TEMP,HUMID};

#define noACK 0
#define ACK   1    

//Sensor  opcodes               adr command r/w
#define STATUS_REG_W 0x06   //  000  0011   0
#define STATUS_REG_R 0x07   //  000  0011   1
#define MEASURE_TEMP 0x03   //  000  0001   1
#define MEASURE_HUMI 0x05   //  000  0010   1
#define RESET        0x1E   //  000  1111   0

//Function definitions
unsigned char s_write_byte (unsigned char value);
unsigned char s_read_byte (unsigned char ack);
void s_transstart (void);
void s_connectionreset (void);
unsigned char s_softreset (void);
unsigned char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum);
unsigned char s_write_statusreg(unsigned char *p_value);
unsigned char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
/** \brief calculate temperature degreesC and humidity %RH
	\param *p_humidity pointer to RH to recalculate (12 bit ticks to %RH)
	\param *p_temperature pointer to temp. to recalculate (14 bit ticks to deg)
	\param D1 temperature correction depending on supply voltage (DS 4.3, table 9)
*/
void calc_sht11(float *p_humidity, float *p_temperature, float d1);

#endif
