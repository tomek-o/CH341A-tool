/** \file
	\brief TEA5767
*/

#include "TEA5767.h"
#include "Log.h"
#include "CH341A.h"
#include "common/StaticCheck.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define PROMPT "TEA5767: "

//#define LOCAL_DEBUG
#ifdef LOCAL_DEBUG
/** \note there are some compiler crashes / internal errors when using LOG directly,
	thus redundant function was defined here
*/
void LOCAL_LOG(char *lpData, ...)
{
	va_list ap;
	char buf[2048]; //determines max message length

	int size = 0;
	if ((int)sizeof(buf)-size-2 > 0)
	{
		va_start(ap, lpData);
		size += vsnprintf(buf + size, sizeof(buf)-size-2, lpData, ap);
		va_end(ap);
	}
	if (size > (int)sizeof(buf) - 2)
		size = sizeof(buf) - 2;

	buf[size] = '\0';
	LOG("%s", buf);
}
#else
#define LOCAL_LOG(...)
#endif

/** \note Do not mistake with 1100000 (7-bit adressing style) in TEA5767 datasheet
*/
#define SLA_W (0xC0)		///< I2C write address
#define SLA_R 0xC0 //(SLA_W | 0x01)	///< I2C read address

static unsigned char write_bytes[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char read_bytes[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint32_t tune = 99900UL;

////////////////////////////////////////////////////////////////////////
// WRITE REGISTERS
////////////////////////////////////////////////////////////////////////

// REGISTER W1
#define TEA5767_MUTE		0x80	///< if MUTE = 1 then L and R audio are muted
#define TEA5767_SEARCH		0x40	///< Search mode: if SM = 1 then in search mode
//	Bits 0-5 for divider MSB (PLL 13:8)

// REGISTER W2
// 	Bits 0-7 for divider LSB (PLL 7:0) 

// REGISTER W3
#define TEA5767_SUD	0x80			///< Search Up/Down: if SUD = 1 then search up; if SUD = 0 then search down
/// Search stop levels:
#define TEA5767_SRCH_HIGH_LVL	0x60///< ADC output = 10
#define TEA5767_SRCH_MID_LVL	0x40///< ADC output = 7
#define TEA5767_SRCH_LOW_LVL	0x20///< ADC output = 5
#define TEA5767_HIGH_LO_INJECT	0x10///< High/Low Side Injection
#define TEA5767_MONO		0x08	///< Force mono
#define TEA5767_MUTE_RIGHT	0x04	///< Mute right channel and force mono
#define TEA5767_MUTE_LEFT	0x02	///< Mute left channel and force mono
#define TEA5767_PORT1_HIGH	0x01	///< Software programmable port 1: if SWP1 = 1 then port 1 is HIGH; if SWP1 = 0 then port 1 is LOW

// REGISTER W4
#define TEA5767_PORT2_HIGH	0x80	///< Software programmable port 2: if SWP2 = 1 then port 2 is HIGH; if SWP2 = 0 then port 2 is LOW
#define TEA5767_STDBY		0x40	///< Standby: if STBY = 1 then in Standby mode (I2C remains active)
#define TEA5767_BAND_LIMIT	0x20	///< Band Limits: BL = 1 => Japan 76-108 MHz; BL = 0 => US/EU 87.5-108
#define TEA5767_XTAL		0x10	///< Set to 1 for 32.768 kHz XTAL
#define TEA5767_SOFT_MUTE	0x08	///< Mutes low signal
#define TEA5767_HCC			0x04	///< High Cut Control, gives the possibility to cut high frequencies
									///< from the audio signal when a weak signal is received
#define TEA5767_SNC			0x02	///< Stereo Noise Cancelling
#define TEA5767_SRCH_IND	0x01	///< Search Indicator: if SI = 1 then pin SWPORT1 is output for the ready
									///< flag; if SI = 0 then pin SWPORT1 is software programmable port 1
// REGISTER W5
#define TEA5767_PLLREF		0x80	///< Set to 0 for 32.768 kHz XTAL
#define TEA5767_DTC			0X40	///< if DTC = 1 then the de-emphasis time constant is 75 µs; if DTC = 0
									///< then the de-emphasis time constant is 50 µs
									///< Europe: used 50 us


////////////////////////////////////////////////////////////////////////
// READ REGISTERS
////////////////////////////////////////////////////////////////////////

// REGISTER R1
#define TEA5767_READY_FLAG	0x80	///< Ready Flag: if RF = 1 then a station has been found or the band limit
									///< has been reached; if RF = 0 then no station has been found
#define TEA5767_BAND_LIMIT_FLAG	0X40///< Band Limit Flag: if BLF = 1 then the band limit has been reached; if
									///< BLF = 0 then the band limit has not been reached
//  bits 5...0: PLL[13:8] setting of synthesizer programmable counter after search or preset

// REGISTER R2
//  bits 7...0: PLL[7:0] setting of synthesizer programmable counter after search or preset

// REGISTER R3
#define TEA5767_STEREO		0x80	///< stereo indicator
#define TEA5767_PLL			0x7f	///< IF counter result

// REGISTER R4
#define TEA5767_ADC_LEVEL	0xf0	///< level ADC output
#define TEA5767_CHIP_ID		0x0f	///< Chip Identification: these bits have to be set to logic 0

// REGISTER R5
/// - reserved for future use -


int TEA5767_write(void)
{
	uint8_t buf[6];
	STATIC_CHECK(sizeof(buf) == 1 + sizeof(write_bytes), BufferSizeMismatch);
	buf[0] = SLA_W;
	memcpy(buf + 1, write_bytes, sizeof(write_bytes));
	return ch341a.I2CWriteRead(buf, sizeof(buf), NULL, 0);
}

int TEA5767_read(void)  // Odczyt danych z TEA5767
{
	uint8_t addr = SLA_R;
	return ch341a.I2CWriteRead(&addr, 1, read_bytes, sizeof(read_bytes));
}

int TEA5767_init(void)
{
	write_bytes[0] = 0x2F;  // default: 99.9 MHz
	write_bytes[1] = 0x87;
#if 1
	write_bytes[2] = TEA5767_SUD | TEA5767_SRCH_LOW_LVL | TEA5767_MONO;
#else
	write_bytes[2] = TEA5767_SUD | TEA5767_SRCH_MID_LVL;
#endif
	write_bytes[3] = TEA5767_XTAL;                                                                  
	write_bytes[4] = 0x00;
	/** \todo chip detection/identification */
	return 0;
}

void TEA5767_tune(uint32_t value)
{
	// reference frequency = 32768 Hz
	// low side injection formula
	LOCAL_LOG("Tune to %u.%02u MHz\n", (unsigned int)(value/1000), (unsigned int)(value%1000)/10);
	uint16_t n = static_cast<uint16_t>((uint32_t)4*(value*1000 - 225000) >> 15);
	write_bytes[0] = static_cast<uint8_t>((write_bytes[0] & 0xC0) | (n >> 8));
	write_bytes[1] = static_cast<uint8_t>(n & 0xFF);
	tune = value;
}

uint32_t TEA5767_get_tune(void)
{
	return tune;
}

void TEA5767_search(bool up)
{
	if (up)
	{
		LOCAL_LOG("Search up\n");
		write_bytes[2] |= TEA5767_SUD;
		TEA5767_tune(tune+150);
	}
	else
	{
		LOCAL_LOG("Search down\n");
		write_bytes[2] &= ~TEA5767_SUD;		
		TEA5767_tune(tune-150);
	}
	write_bytes[0] |= TEA5767_SEARCH | TEA5767_MUTE;
	TEA5767_write();
}

void TEA5767_exit_search(void)
{
	write_bytes[0] = static_cast<uint8_t>(read_bytes[0] & 0x3f);
	write_bytes[1] = read_bytes[1];
	write_bytes[0] &= ~(TEA5767_SEARCH | TEA5767_MUTE);
	TEA5767_write();
	tune = ((((read_bytes[0]&0x3F)<<8)+read_bytes[1])*32768/4 + 225000)/1000;
	LOCAL_LOG("Exit search, tuned = %u.%u MHz\n", (unsigned int)(tune/1000), (unsigned int)(tune%10));
}
	
int TEA5767_get_status(struct TEA5767_status *status)
{
	TEA5767_read();
	//LOCAL_LOG("read_bytes %02X %02X %02X %02X %02X\n", read_bytes[0], read_bytes[1], read_bytes[2], read_bytes[3], read_bytes[4]);
	memset(status, 0, sizeof(*status));

	uint32_t freq = ((((read_bytes[0]&0x3F)<<8)+read_bytes[1])*32768/4 + 225000)/1000;
	//LOCAL_LOG("Freq = %u.%u MHz\n", (unsigned int)(freq/1000), (unsigned int)(freq%1000));

	if (read_bytes[0] & TEA5767_READY_FLAG) /* ready */
	{
		//LOCAL_LOG("Ready\n");
		status->ready = 1;
		uint8_t val = static_cast<uint8_t>(read_bytes[2] & 0x7F); /* IF counter */
		if (abs(val - 0x36) < 2) 			 /* close match */
		{
			//LOCAL_LOG("Tuned!\n");
			status->tuned = 1;
		}
	}
	if (read_bytes[0] & TEA5767_BAND_LIMIT_FLAG)
	{
		LOCAL_LOG("Band limit reached\n");
		status->band_limit = 1;
	}
	if (read_bytes[2] & TEA5767_STEREO)
	{
		//LOCAL_LOG("Stereo reception\n");
		status->stereo = 1;
	}
	status->rx_power = read_bytes[3] >> 4;
	//LOCAL_LOG("rx_power = %d\n", status->rx_power);
	status->frequency = freq;
	return 0;
}

