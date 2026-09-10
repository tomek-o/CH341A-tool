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

/** Enable/disable RDS reception (R2_RDS_EN) and reset any previously decoded RDS data */
void RDA5807M_set_rds_enabled(bool on);

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

struct RDA5807M_rds_status
{
	bool valid;	// at least one RDS group decoded since the last tune/search/init; if false, all fields below are meaningless
	uint16_t piCode;
	uint8_t groupType;			// RDS group type, 0-15
	bool groupVersionB;			// false = version A, true = version B
	bool trafficProgram;		// TP flag
	uint8_t programType;		// PTY, 0-31
	bool trafficAnnouncement;	// TA flag, only meaningful once psReady/group 0 seen
	bool psReady;
	char programService[9];	// station name (PS), NUL-terminated
	bool rtReady;
	char radioText[65];			// RadioText (RT), NUL-terminated
};

/** Retrieves currently decoded RDS data (accumulated from groups seen so far since the last
	tune/search/init). Does not perform I2C communication - call RDA5807M_get_status() first
	to receive and decode any pending RDS group. */
uint8_t RDA5807M_get_rds(struct RDA5807M_rds_status *rds);

void RDA5807M_mute(bool state);

void RDA5807M_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
