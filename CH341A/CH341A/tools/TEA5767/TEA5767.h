/** \file
	\brief TEA5767 FM radio
	
	TEA5767 FM tuner. Tested with module from Philips SA3115/02 mp3 player in I2C mode.
	This module is using XTAL 32768.
	Module markings:
		FXO 55D(H)
		M230-55D(H)
		DS206
	
	Prerequisitions: configured I2C (TWI) hardware
*/
#ifndef Tea5767H
#define Tea5767H

#include <stdint.h>

int TEA5767_init(void);

/** \param value Tuned frequency in kHz */
void TEA5767_tune(uint32_t value);
uint32_t TEA5767_get_tune(void);

void TEA5767_search(bool up);
void TEA5767_exit_search(void);
int TEA5767_write(void);


struct TEA5767_status
{
	uint32_t frequency;
	uint8_t ready;
	uint8_t band_limit;
	uint8_t tuned;
	uint8_t stereo;
	unsigned int rx_power;
};

int TEA5767_get_status(struct TEA5767_status *status);

#endif
