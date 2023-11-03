#ifndef Rda5807mH
#define Rda5807mH

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t RDA5807M_init(void);

void RDA5807M_search(uint8_t up);

/** \param value Tuned frequency, unit = 10kHz */
void RDA5807M_tune(uint16_t value);

void RDA5807M_set_volume(uint8_t volume);

void RDA5807M_set_stereo(uint8_t on);

struct RDA5807M_status
{
	bool valid;
	unsigned int frequency;	// unit = 10kHz
	bool stereo;
	unsigned int rssi;
	bool tuneok;
	bool fmtrue;
	bool fmready;
	bool tunefail;
};

uint8_t RDA5807M_get_status(struct RDA5807M_status *status);

void RDA5807M_mute(bool state);

void RDA5807M_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
