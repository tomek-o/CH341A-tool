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

#include <vcl.h>
#pragma hdrstop

#include "kt0803l.h"
#include "Log.h"
#include "CH341A.h"

#define DBG LOG
#pragma warn -8071

namespace {

	int readReg(unsigned char regAddr, unsigned char *pData) {
		return ch341a.I2CWriteCommandReadByte(KT0803L_ADDR, regAddr, *pData);
	}

	int writeReg(unsigned char regAddr, unsigned char data) {
		int status = ch341a.I2CWriteCommandWriteByte(KT0803L_ADDR, regAddr, data);
		Sleep(10);
		return status;
	}

	void SleepWithMsgProcessing(int ms)
	{
		do
		{
			Sleep(20);
			Application->ProcessMessages();
			ms -= 20;
		} while (ms > 0);
	}

}

void KT0803L_dump(void) {
	unsigned char regs[]={
		KT0803L_REG_ADDR_00, 
		KT0803L_REG_ADDR_01, 
		KT0803L_REG_ADDR_02, 	
		KT0803L_REG_ADDR_04, 	
		KT0803L_REG_ADDR_0B, 	
		KT0803L_REG_ADDR_0C, 	
		KT0803L_REG_ADDR_0E, 	
		KT0803L_REG_ADDR_0F, 	
		KT0803L_REG_ADDR_10, 	
		KT0803L_REG_ADDR_12, 	
		KT0803L_REG_ADDR_13, 	
		KT0803L_REG_ADDR_14, 	
		KT0803L_REG_ADDR_15, 	
		KT0803L_REG_ADDR_16, 	
		KT0803L_REG_ADDR_17, 	
		KT0803L_REG_ADDR_1E, 	
		KT0803L_REG_ADDR_26, 	
		KT0803L_REG_ADDR_27
	};

	for(unsigned i=0;i<sizeof(regs)/sizeof(regs[0]);i++) {
		unsigned char reg=0;

		readReg(regs[i], &reg);
		DBG("%02X: %02X\n", regs[i], reg);
	}
}

int KT0803L_init(const KT0803L_SETTING *pSetting) {
	KT0803L_REG_0F r0F;
	KT0803L_REG_13 r13;
	KT0803L_REG_1E r1E;
	KT0803L_REG_17 r17;
	KT0803L_REG_02 r02;
	KT0803L_REG_12 r12;
	
	if(!pSetting)
		goto failed;
	
	for (int i=0; i<4; i++) {
		if(readReg(KT0803L_REG_ADDR_0F, &r0F.b)<0)
			goto failed;
		
		if(r0F.f.PW_OK)
			break;

		DBG("KT0803L is not ready yet\n");
		SleepWithMsgProcessing(500);
		if (i == 3)
			goto failed;
	}
	DBG("r0F.f.PW_OK=0x%X\n", r0F.f.PW_OK);

	if(readReg(KT0803L_REG_ADDR_13, &r13.b)<0)
		goto failed;

	//PA structure selection
	r13.f.PA_CTRL=pSetting->useExtInductor?1:0;
	DBG("r13.f.PA_CTRL=0x%X\n", r13.f.PA_CTRL);

	if(writeReg(KT0803L_REG_ADDR_13, r13.b)<0)
		goto failed;
	
	//Read registers
	if(readReg(KT0803L_REG_ADDR_1E, &r1E.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_17, &r17.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_02, &r02.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_12, &r12.b)<0)
		goto failed;

	//Clock selection
	if(!pSetting->clkSetting.isUpToSW) {
		DBG("Clock setting is up to software\n");
		r1E.f.DCLK=1;

		r1E.f.REF_CLK_3_0=pSetting->clkSetting.freq;
		DBG("r1E.f.REF_CLK_3_0=0x%X\n", r1E.f.REF_CLK_3_0);
		if(pSetting->clkSetting.freq==KT0803L_CLK_FREQ_32_768KHZ) {
			DBG("Clock frequency = 32.768KHz\n");
			r17.f.XTAL_SEL=0;
		} else {
			DBG("Clock frequency != 32.768KHz\n");
			r17.f.XTAL_SEL=1;
		}
			
		if(!pSetting->clkSetting.isXTAL) {
			DBG("Use reference clock\n");
			r1E.f.XTALD=1;
		} else {
			DBG("Use XTAL as clock\n");
			r1E.f.XTALD=0;
		}
	} else {
		DBG("Clock setting is up to SW1/SW2 pin\n");
		r1E.f.DCLK=0;
	}
		
	//Pilot tone amplitude adjustment
	r02.f.PLTADJ=pSetting->isPLTAmpHigh?1:0;
	DBG("r02.f.PLTADJ=0x%X\n", r02.f.PLTADJ);
	
	//Pre-emphasis time constant set
	r02.f.PHTCNST=pSetting->isPHTCNST50us?1:0;
	DBG("r02.f.PHTCNST=0x%X\n", r02.f.PHTCNST);
	
	//Frequency deviation detection
	r17.f.FDEV=pSetting->isFDEV112_5KHZ?1:0;
	DBG("r17.f.FDEV=0x%X\n", r17.f.FDEV);

	//Channel switching behavior selection
	r12.f.SW_MOD=pSetting->isCHSELPAOff?1:0;
	DBG("r12.f.SW_MOD=0x%X\n", r12.f.SW_MOD);
	
	//Write registers
	if(writeReg(KT0803L_REG_ADDR_1E, r1E.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_17, r17.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_02, r02.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_12, r12.b)<0)
		goto failed;

	return KT0803L_OK;

failed:
	DBG("Failed\n");
	return KT0803L_ERR_FAILED;
}

//Frequency related
int KT0803L_setFreq(unsigned short freq) {
	KT0803L_REG_00 r00;
	KT0803L_REG_01 r01;
	KT0803L_REG_02 r02;
	
	freq*=2;
	
	if(readReg(KT0803L_REG_ADDR_00, &r00.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_01, &r01.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_02, &r02.b)<0)
		goto failed;
	
	r00.f.CHSEL_8_1=(unsigned char)((freq&0x01fe)>>1);
	r01.f.CHSEL_11_9=(unsigned char)((freq&0x0e00)>>9);
	r02.f.CHSEL_0=(unsigned char)(freq&0x0001);
	
	if(writeReg(KT0803L_REG_ADDR_00, r00.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_01, r01.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_02, r02.b)<0)
		goto failed;

	
	return KT0803L_OK;

failed:
	DBG("Failed\n");
	return KT0803L_ERR_FAILED;
}

int KT0803L_getFreq(unsigned short *pFreq) {
	KT0803L_REG_00 r00;
	KT0803L_REG_01 r01;
	KT0803L_REG_02 r02;
	
	if(!pFreq)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_00, &r00.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_01, &r01.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_02, &r02.b)<0)
		goto failed;

	*pFreq=r02.f.CHSEL_0;
	*pFreq|=((unsigned short)r00.f.CHSEL_8_1)<<1;
	*pFreq|=((unsigned short)r01.f.CHSEL_11_9)<<9;
	*pFreq/=2;

	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

//Output related
int KT0803L_setRFGain(unsigned char gain, KT0803L_BOOL isPABias) {
	KT0803L_REG_01 r01;
	KT0803L_REG_02 r02;
	KT0803L_REG_13 r13;
	KT0803L_REG_0E r0E;
	
	if(readReg(KT0803L_REG_ADDR_01, &r01.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_02, &r02.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_13, &r13.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_0E, &r0E.b)<0)
		goto failed;

	r01.f.RFGAIN_1_0=gain&0x03;
	r02.f.RFGAIN_3=(gain&0x08)>>3;
	r13.f.RFGAIN_2=(gain&0x04)>>2;
	r0E.f.PA_BIAS=isPABias?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_01, r01.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_02, r02.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_13, r13.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_0E, r0E.b)<0)
		goto failed;

	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_getRFGain(unsigned char *pGain, KT0803L_BOOL *pIsPABias) {
	KT0803L_REG_01 r01;
	KT0803L_REG_02 r02;
	KT0803L_REG_13 r13;
	KT0803L_REG_0E r0E;
	
	if(readReg(KT0803L_REG_ADDR_01, &r01.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_02, &r02.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_13, &r13.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_0E, &r0E.b)<0)
		goto failed;

	*pGain=r01.f.RFGAIN_1_0;
	*pGain|=r02.f.RFGAIN_3<<3;
	*pGain|=r13.f.RFGAIN_2<<2;
	*pIsPABias=r0E.f.PA_BIAS?KT0803L_TRUE:KT0803L_FALSE;

	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_setAutoPADown(KT0803L_BOOL flag) {
	KT0803L_REG_0B r0B;
	
	if(readReg(KT0803L_REG_ADDR_0B, &r0B.b)<0)
		goto failed;
	
	r0B.f.AUTO_PADN=flag?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_0B, r0B.b)<0)
		goto failed;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_isAutoPADown(KT0803L_BOOL *pFlag) {
	KT0803L_REG_0B r0B;
	
	if(!pFlag)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_0B, &r0B.b)<0)
		goto failed;
	
	*pFlag=r0B.f.AUTO_PADN?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_PADown(KT0803L_BOOL flag) {
	KT0803L_REG_0B r0B;
	
	if(readReg(KT0803L_REG_ADDR_0B, &r0B.b)<0)
		goto failed;
	
	r0B.f.PDPA=flag?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_0B, r0B.b)<0)
		goto failed;
	
	return KT0803L_OK;

failed:
	DBG("Failed\n");
	return KT0803L_ERR_FAILED;
}

int KT0803L_isPADown(KT0803L_BOOL *pFlag) {
	KT0803L_REG_0B r0B;
	
	if(!pFlag)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_0B, &r0B.b)<0)
		goto failed;
	
	*pFlag=r0B.f.PDPA?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_mute(KT0803L_BOOL flag) {
	KT0803L_REG_02 r02;
	
	if(readReg(KT0803L_REG_ADDR_02, &r02.b)<0)
		goto failed;
	
	r02.f.MUTE=flag?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_02, r02.b)<0)
		goto failed;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_isMute(KT0803L_BOOL *pFlag) {
	KT0803L_REG_02 r02;
	
	if(!pFlag)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_02, &r02.b)<0)
		goto failed;
	
	*pFlag=r02.f.MUTE?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_setMono(KT0803L_BOOL flag) {
	KT0803L_REG_04 r04;
	
	if(readReg(KT0803L_REG_ADDR_04, &r04.b)<0)
		goto failed;
	
	r04.f.MONO=flag?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_04, r04.b)<0)
		goto failed;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_isMono(KT0803L_BOOL *pFlag) {
	KT0803L_REG_04 r04;
	
	if(!pFlag)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_04, &r04.b)<0)
		goto failed;
	
	*pFlag=r04.f.MONO?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_setBBLevel(unsigned char level) {
	KT0803L_REG_04 r04;
	
	if(readReg(KT0803L_REG_ADDR_04, &r04.b)<0)
		goto failed;
	
	r04.f.BASS_1_0=level;
	
	if(writeReg(KT0803L_REG_ADDR_04, r04.b)<0)
		goto failed;

	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_getBBLevel(unsigned char *pLevel) {
	KT0803L_REG_04 r04;
	
	if(!pLevel)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_04, &r04.b)<0)
		goto failed;
	
	*pLevel=r04.f.BASS_1_0;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_enableAUENHANCE(KT0803L_BOOL flag) {
	KT0803L_REG_17 r17;
	
	if(readReg(KT0803L_REG_ADDR_17, &r17.b)<0)
		goto failed;

	r17.f.AU_ENHANCE=flag?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_17, r17.b)<0)
		goto failed;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_isAUENHANCEEnable(KT0803L_BOOL *pFlag) {
	KT0803L_REG_17 r17;
	
	if(!pFlag)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_17, &r17.b)<0)
		goto failed;

	*pFlag=r17.f.AU_ENHANCE?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

//Input related
int KT0803L_setALCSetting(const KT0803L_ALC_SETTING *pSetting) {
	KT0803L_REG_0C r0C;
	KT0803L_REG_15 r15;
	KT0803L_REG_26 r26;
	KT0803L_REG_27 r27;

	if(!pSetting)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_0C, &r0C.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_15, &r15.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_26, &r26.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_27, &r27.b)<0)
		goto failed;

	r0C.f.ALC_ATTACH_TIME_3_0=pSetting->attackTime;
	r0C.f.ALC_DECAY_TIME_3_0=pSetting->decayTime;
	r15.f.ALCCMPGAIN_2_0=pSetting->ALCCMPGain;
	r26.f.ALCHOLD_2_0=pSetting->holdTime;
	r27.f.ALCLOWTH_3_0=pSetting->lowThreshold;
	r26.f.ALCHIGHTH_2_0=pSetting->highThreshold;
	
	if(writeReg(KT0803L_REG_ADDR_0C, r0C.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_15, r15.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_26, r26.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_27, r27.b)<0)
		goto failed;

	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_getALCSetting(KT0803L_ALC_SETTING *pSetting) {
	KT0803L_REG_0C r0C;
	KT0803L_REG_15 r15;
	KT0803L_REG_26 r26;
	KT0803L_REG_27 r27;
	
	if(!pSetting)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_0C, &r0C.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_15, &r15.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_26, &r26.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_27, &r27.b)<0)
		goto failed;

	pSetting->attackTime=r0C.f.ALC_ATTACH_TIME_3_0;
	pSetting->decayTime=r0C.f.ALC_DECAY_TIME_3_0;
	pSetting->ALCCMPGain=r15.f.ALCCMPGAIN_2_0;
	pSetting->holdTime=r26.f.ALCHOLD_2_0;
	pSetting->lowThreshold=r27.f.ALCLOWTH_3_0;
	pSetting->highThreshold=r26.f.ALCHIGHTH_2_0;

	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_enableALC(KT0803L_BOOL flag) {
	KT0803L_REG_04 r04;
	
	if(readReg(KT0803L_REG_ADDR_04, &r04.b)<0)
		goto failed;
	
	r04.f.ALC_EN=flag?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_04, r04.b)<0)
		goto failed;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_isALCEnable(KT0803L_BOOL *pFlag) {
	KT0803L_REG_04 r04;
	
	if(!pFlag)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_04, &r04.b)<0)
		goto failed;
	
	*pFlag=r04.f.ALC_EN?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_setPGAGain(unsigned char gain, KT0803L_BOOL is1dBStep) {
	KT0803L_REG_01 r01;
	KT0803L_REG_04 r04;
	KT0803L_REG_10 r10;
	
	if(readReg(KT0803L_REG_ADDR_01, &r01.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_04, &r04.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_10, &r10.b)<0)
		goto failed;
	
	r01.f.PGA_2_0=(gain&0x1c)>>2;
	r04.f.PGA_LSB_1_0=gain&0x3;
	r10.f.PGAMOD=is1dBStep?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_01, r01.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_04, r04.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_10, r10.b)<0)
		goto failed;

	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_getPGAGain(unsigned char *pGain, KT0803L_BOOL *pIs1dBStep) {
	KT0803L_REG_01 r01;
	KT0803L_REG_04 r04;
	KT0803L_REG_10 r10;
	
	if(readReg(KT0803L_REG_ADDR_01, &r01.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_04, &r04.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_10, &r10.b)<0)
		goto failed;

	*pGain=r04.f.PGA_LSB_1_0;
	*pGain|=r01.f.PGA_2_0<<2;
	*pIs1dBStep=r10.f.PGAMOD?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_setSDSetting(const KT0803L_SD_SETTING *pSetting) {
	KT0803L_REG_12 r12;
	KT0803L_REG_14 r14;
	KT0803L_REG_16 r16;
	
	if(!pSetting)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_12, &r12.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_14, &r14.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_16, &r16.b)<0)
		goto failed;

	r14.f.SLNCTIME_2_0=pSetting->window&0x07;
	r14.f.SLNCTIME_3=(pSetting->window&0x08)>>3;
	r12.f.SLNCTHL_2_0=pSetting->lowThreshold;
	r12.f.SLNCTHH_2_0=pSetting->highThreshold;
	r16.f.SLNCCNTLOW_2_0=pSetting->lowCount;
	r14.f.SLNCCNTHIGH_2_0=pSetting->highCount;
	
	if(writeReg(KT0803L_REG_ADDR_12, r12.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_14, r14.b)<0)
		goto failed;
	if(writeReg(KT0803L_REG_ADDR_16, r16.b)<0)
		goto failed;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_getSDSetting(KT0803L_SD_SETTING *pSetting) {
	KT0803L_REG_12 r12;
	KT0803L_REG_14 r14;
	KT0803L_REG_16 r16;
	
	if(!pSetting)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_12, &r12.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_14, &r14.b)<0)
		goto failed;
	if(readReg(KT0803L_REG_ADDR_16, &r16.b)<0)
		goto failed;

	memset(pSetting, 0, sizeof(KT0803L_SD_SETTING));
	pSetting->window=r14.f.SLNCTIME_2_0;
	pSetting->window|=r14.f.SLNCTIME_3<<3;
	pSetting->lowThreshold=r12.f.SLNCTHL_2_0;
	pSetting->highThreshold=r12.f.SLNCTHH_2_0;
	pSetting->lowCount=r16.f.SLNCCNTLOW_2_0;
	pSetting->highCount=r14.f.SLNCCNTHIGH_2_0;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_disableSD(KT0803L_BOOL flag) {
	KT0803L_REG_12 r12;
	
	if(readReg(KT0803L_REG_ADDR_12, &r12.b)<0)
		goto failed;

	r12.f.SLNCDIS=flag?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_12, r12.b)<0)
		goto failed;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_isSDDisable(KT0803L_BOOL *pFlag) {
	KT0803L_REG_12 r12;
	
	if(!pFlag)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_12, &r12.b)<0)
		goto failed;

	*pFlag=r12.f.SLNCDIS?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

//Power related
int KT0803L_standby(KT0803L_BOOL flag) {
	KT0803L_REG_0B r0B;
	
	if(readReg(KT0803L_REG_ADDR_0B, &r0B.b)<0)
		goto failed;

	r0B.f.STANDBY=flag?1:0;
	
	if(writeReg(KT0803L_REG_ADDR_0B, r0B.b)<0)
		goto failed;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

int KT0803L_isStandby(KT0803L_BOOL *pFlag) {
	KT0803L_REG_0B r0B;
	
	if(!pFlag)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_0B, &r0B.b)<0)
		goto failed;

	*pFlag=r0B.f.STANDBY?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}

//Status related
int KT0803L_getStatus(KT0803L_BOOL *pIsPwrOk, KT0803L_BOOL *pIsSilence) {
	KT0803L_REG_0F r0F;
	
	if(!pIsPwrOk || !pIsSilence)
		goto failed;
	
	if(readReg(KT0803L_REG_ADDR_0F, &r0F.b)<0)
		goto failed;

	*pIsPwrOk=r0F.f.PW_OK?KT0803L_TRUE:KT0803L_FALSE;
	*pIsSilence=r0F.f.SLNCID?KT0803L_TRUE:KT0803L_FALSE;
	
	return KT0803L_OK;

failed:
	return KT0803L_ERR_FAILED;
}
