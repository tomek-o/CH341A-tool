/*
 * BSD 3-Clause License
 * 
 * KT0803L driver for ESP8266
 * Copyright (c) 2017, ccbruce0812 (ccbruce@gmail.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef KT0803L_H
#define KT0803L_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kt0803l_reg.h"
#include "kt0803l_data.h"

//KT0803L's I2C addresses
#define KT0803L_ADDR				(0x3e)

void KT0803L_dump(void);

//Initialization
int KT0803L_init(const KT0803L_SETTING *pSetting);

//Frequency related
int KT0803L_setFreq(unsigned short freq);
int KT0803L_getFreq(unsigned short *pFreq);

//Output related
int KT0803L_setRFGain(unsigned char gain, KT0803L_BOOL isPABias);
int KT0803L_getRFGain(unsigned char *pGain, KT0803L_BOOL *pIsPABias);

int KT0803L_setAutoPADown(KT0803L_BOOL flag);
int KT0803L_isAutoPADown(KT0803L_BOOL *pFlag);

int KT0803L_PADown(KT0803L_BOOL flag);
int KT0803L_isPADown(KT0803L_BOOL *pFlag);

int KT0803L_mute(KT0803L_BOOL flag);
int KT0803L_isMute(KT0803L_BOOL *pFlag);

int KT0803L_setMono(KT0803L_BOOL flag);
int KT0803L_isMono(KT0803L_BOOL *pFlag);

int KT0803L_setBBLevel(unsigned char level);
int KT0803L_getBBLevel(unsigned char *pLevel);

int KT0803L_enableAUENHANCE(KT0803L_BOOL flag);
int KT0803L_isAUENHANCEEnable(KT0803L_BOOL *pFlag);

//Input related
int KT0803L_setALCSetting(const KT0803L_ALC_SETTING *pSetting);
int KT0803L_getALCSetting(KT0803L_ALC_SETTING *pSetting);

int KT0803L_enableALC(KT0803L_BOOL flag);
int KT0803L_isALCEnable(KT0803L_BOOL *pFlag);

int KT0803L_setPGAGain(unsigned char gain, KT0803L_BOOL is1dBStep);
int KT0803L_getPGAGain(unsigned char *pGain, KT0803L_BOOL *is1dBStep);

int KT0803L_setSDSetting(const KT0803L_SD_SETTING *pSetting);
int KT0803L_getSDSetting(KT0803L_SD_SETTING *pSetting);

int KT0803L_disableSD(KT0803L_BOOL flag);
int KT0803L_isSDDisable(KT0803L_BOOL *pFlag);

//Power related
int KT0803L_standby(KT0803L_BOOL flag);
int KT0803L_isStandby(KT0803L_BOOL *pFlag);

//Status related
int KT0803L_getStatus(KT0803L_BOOL *pIsPwrOk, KT0803L_BOOL *pIsSilence);

#ifdef __cplusplus
}
#endif

#endif
