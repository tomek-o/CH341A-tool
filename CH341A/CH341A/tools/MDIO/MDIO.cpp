#include "MDIO.h"
#include "CH341A.h"
#include "Log.h"
#include <stdint.h>
#include <windows.h>


#ifdef __BORLANDC__
#pragma warn -8071
#endif


namespace {

void delay()
{
#if 0
	Sleep(2);	///< \todo inaccurate MDIO delay
#else
	LARGE_INTEGER start, stop, frequency;
	// Get the frequency in counts per second.
	if( !QueryPerformanceFrequency(&frequency) )
	{
		Sleep(1);
		return;
	}
	frequency.QuadPart /= 200000;    // frequency in counts per 200 microseconds
	QueryPerformanceCounter(&start);
	while(1)
	{
		QueryPerformanceCounter(&stop);
		if( (stop.QuadPart - start.QuadPart) / frequency.QuadPart >= 1 )
			break;
	}
#endif
}

enum
{
	MDC_PIN_ID = 18,	// MDC = SCL, output only
	MDIO_OUT_PIN_ID = 19,	// MDIO = SDA, semi-bidirectional with open-drain output and pull-up
	MDIO_IN_PIN_ID = 23		// MDIO = SDA, separate bit required for reading input
};

/* 	For testing with STM Nucleo H755ZI board:
	MDIO = JP7 (jumper on the top side), left side (closer to U15/PHY)
	MDC = SB84 (0 Ohm resistor on the bottom side)
    STM erased, to be extra safe 220 Ohm resistors were used in series with MDIO and MDC lines.
*/

bool mdcState = false;
bool mdioDirOut = false;
bool mdioState = false;

void update(void)
{
	uint32_t direction = (1u << MDC_PIN_ID);
	if (mdioDirOut)
		direction |= (1u << MDIO_OUT_PIN_ID);
	uint32_t value = 0;
	if (mdcState)
		value |= (1u << MDC_PIN_ID);
	if (mdioState || !mdioDirOut)	/* open drain */
		value |= (1u << MDIO_OUT_PIN_ID);

	ch341a.SetGpioOutputs(direction, value);
}

void BSP_SetPinMDC(bool set)
{
	mdcState = set;
	update();
}



uint16_t BSP_GetPinMDInput(void)
{
	uint32_t dataIn = 0;

	int status = ch341a.GetGpioInputs(dataIn);
	if (status != 0)
	{
		//LOG("Reading inputs failed!\n");
		return 0;
	}
	if (dataIn & (1u << MDIO_IN_PIN_ID))
		return 1;
	return 0;
}

void BSP_ChangeMDIPinDir(bool output)
{
#if 0
	mdioDirOut = output;
#else
	// 3-state SDA
	mdioDirOut = true;
	if (output) {

	} else {
		mdioState = true;
	}
#endif

	update();
}

void BSP_SetPinMDIO(bool set)
{
	mdioState = set;
	update();
}


void bitsOut(uint32_t  value, int32_t nbBits)
{
	for(int32_t i=0; i < nbBits; i++) {
        if ((value & (0x80000000u >> i)) != 0) {
            BSP_SetPinMDIO(true);
        } else {
            BSP_SetPinMDIO(false);
        }

        delay ();
        BSP_SetPinMDC(true);
        delay ();
        BSP_SetPinMDC(false);
    }
}

uint16_t bitsIn (void)
{
    /* Input a value from the MII PHY management interface. */
    unsigned int i,val = 0;

    BSP_SetPinMDC(false);
    delay ();
    for (i = 0; i < 16; i++) {
        val <<= 1;
        BSP_SetPinMDC(true);
        delay ();
        BSP_SetPinMDC(false);
        delay ();
        val |= BSP_GetPinMDInput();
    }
    BSP_SetPinMDC(true);
    delay ();
    BSP_SetPinMDC(false);

    return (val & 0xFFFF);
}

}	// namespace


int MDIO_Init(void)
{
	if (!ch341a.IsOpened())
	{
		//LOG("MDIO_Init: CH341A is not opened!\n");
		return -1;
	}
	mdioState = false;
	int status = ch341a.SetGpioOutputs(1u << MDC_PIN_ID, 0x00000000);

	return status;
}

uint16_t MDIO_ReadRegister(uint32_t PHY_Address, uint32_t PHY_Register)
{
    int32_t i = 0;
    uint8_t tempBuffer[8] = {0};

    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;

    /* ST('01) + OP('10) + PHYAD(4 MSB) */
    tempBuffer[i] = 0x60;
    tempBuffer[i++] |= (PHY_Address & 0x1F) >> 1;

	/* PHYAD(1 LSB) + REGADD (5) + 2 TA('00) */
    tempBuffer[i] = (PHY_Address & 0x01) << 7 ;
    tempBuffer[i] |= (PHY_Register & 0x1F) << 2;
    tempBuffer[i++] |= ( 0x03) ;//3
    tempBuffer[i++] = 0xFF;
    tempBuffer[i++] = 0xFF;
    (void)i;

    BSP_ChangeMDIPinDir(true);//output
	uint32_t temp = (tempBuffer[0] << 24) | (tempBuffer[1] <<16) | (tempBuffer[2] << 8) | tempBuffer[3];
    bitsOut(temp, 32);
    temp = (tempBuffer[4] << 24) | (tempBuffer[5] <<16) | (tempBuffer[6] << 8) | tempBuffer[7];
    bitsOut(temp, 15);
	BSP_ChangeMDIPinDir(false);//input

	return bitsIn();
}

void MDIO_WriteRegister(uint32_t PHY_Address, uint32_t PHY_Register, uint32_t value)
{
    int32_t i = 0;
    uint8_t tempBuffer[8] = {0};

    uint32_t temp;

	/*Preamble*/
	tempBuffer[i++] = 0xff;
	tempBuffer[i++] = 0xff;
	tempBuffer[i++] = 0xff;
	tempBuffer[i++] = 0xff;

    /* ST('01) + OP('01) + PHYAD(4 MSB) */
    tempBuffer[i] = 0x50 ;
    tempBuffer[i++] |= (PHY_Address & 0x1F) >> 1;

    /* PHYAD(1 LSB) + REGADD (5) + 2 TA('10) */
    tempBuffer[i] = (PHY_Address & 0x01) << 7 ;
    tempBuffer[i] |= (PHY_Register & 0x1F) << 2;
    tempBuffer[i++] |=  0x2 ;

    /* DATA */
    tempBuffer[i++] = value >> 8;
	tempBuffer[i++] = value & 0xFF;

	(void)i;

    BSP_ChangeMDIPinDir(true);//output
    temp = (tempBuffer[0] << 24) | (tempBuffer[1] <<16) | (tempBuffer[2] << 8) | tempBuffer[3];
    bitsOut(temp, 32);

    temp = (tempBuffer[4] << 24) | (tempBuffer[5] <<16) | (tempBuffer[6] << 8) | tempBuffer[7];
    bitsOut(temp, 32);
    BSP_ChangeMDIPinDir(false);//input
}



static uint32_t mdioRead45Clause(uint8_t phyAddr, uint8_t devType)
{
    int32_t i = 0;
    uint8_t tempBuffer[8] = {0};

	uint32_t temp;

	/*Preamble*/
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;

    /* ST('00) + OP('11) + PHYAD(4 MSB) */
    tempBuffer[i] = 0x30;
    tempBuffer[i++] |= (phyAddr & 0x1F) >> 1;

    /* PHYAD(1 LSB) + DEVTYPE (4) + 2 TA('00) */
    tempBuffer[i] = (phyAddr & 0x01) << 7 ;
    tempBuffer[i] |= (devType & 0x1F) << 2;
    tempBuffer[i++] |= ( 0x03) ;
    tempBuffer[i++] = 0xFF;
	tempBuffer[i++] = 0xFF;

	(void)i;

    BSP_ChangeMDIPinDir(true);//output
    temp = (tempBuffer[0] << 24) | (tempBuffer[1] <<16) | (tempBuffer[2] << 8) | tempBuffer[3];
    bitsOut(temp, 32);
    temp = (tempBuffer[4] << 24) | (tempBuffer[5] <<16) | (tempBuffer[2] << 6) | tempBuffer[7];
    bitsOut(temp, 15); //TODO : changed from 15 to 16
    BSP_ChangeMDIPinDir(false);//input

	return bitsIn();
}

static uint32_t mdioWrite45Clause(uint8_t phyAddr, uint8_t devType, uint16_t phyData )
{
    int32_t i = 0;
    uint8_t tempBuffer[8] = {0};
    uint32_t preamble = 0xffffffff;
    uint32_t temp;

    /*Preamble*/
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;

    /* ST('00) + OP('01) + PHYAD(4 MSB) */
    tempBuffer[i] = 0x10 ;
    tempBuffer[i++] |= (phyAddr & 0x1F) >> 1;

    /* PHYAD(1 LSB) + DEVTYPE (4) + 2 TA('10) */
    tempBuffer[i] = (phyAddr & 0x01) << 7 ;
    tempBuffer[i] |= (devType & 0x1F) << 2;
    tempBuffer[i++] |=  0x2 ;

    /* DATA */
    tempBuffer[i++] = phyData >> 8;
    tempBuffer[i++] = phyData & 0xFF;

    (void)i;

    BSP_ChangeMDIPinDir(false);//input
    BSP_ChangeMDIPinDir(true);//output
    temp = (tempBuffer[0] << 24) | (tempBuffer[1] <<16) | (tempBuffer[2] << 8) | tempBuffer[3];
    (void)temp;
	bitsOut(preamble, 32);
    temp = (tempBuffer[4] << 24) | (tempBuffer[5] <<16) | (tempBuffer[6] << 8) | tempBuffer[7];
    bitsOut(temp, 32);

    return 0;
}

static uint32_t mdioAddr45Clause(uint8_t phyAddr, uint8_t devType, uint16_t regAddr )
{
    int32_t i = 0;
    uint8_t tempBuffer[8] = {0};

	uint32_t preamble = 0xffffffff;

    /*Preamble*/
    delay();
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;
    tempBuffer[i++] = 0xff;

    /* ST('00) + OP('00) + PHYAD(4 MSB) */
    tempBuffer[i] = 0x00 ;
    tempBuffer[i++] |= (phyAddr & 0x1F) >> 1;

    /* PHYAD(1 LSB) + DEVTYPE (4) + 2 TA('10) */
    tempBuffer[i] = (phyAddr & 0x01) << 7 ;
    tempBuffer[i] |= (devType & 0x1F) << 2;
    tempBuffer[i++] |=  0x2 ;

    /* DATA */
    tempBuffer[i++] = regAddr >> 8;
    tempBuffer[i++] = regAddr & 0xFF;

	(void)i;

    BSP_ChangeMDIPinDir(false);//input
    BSP_ChangeMDIPinDir(true);//output
    uint32_t temp = (tempBuffer[4] << 24) | (tempBuffer[5] <<16) | (tempBuffer[6] << 8) | tempBuffer[7];
    BSP_ChangeMDIPinDir(true);//output
    bitsOut(preamble, 32);
    bitsOut(temp, 32);
    BSP_ChangeMDIPinDir(false);//input


    return 0;
}

uint32_t MDIO_ReadRegisterClause45(uint32_t PHY_Address, uint8_t devType, uint32_t addr)
{
    mdioAddr45Clause(PHY_Address, devType, addr);
    return mdioRead45Clause(PHY_Address, devType);
}

void MDIO_WriteRegisterClause45(uint32_t PHY_Address, uint8_t devType, uint32_t addr, uint32_t value)
{
    mdioAddr45Clause(PHY_Address, devType, addr);
    mdioWrite45Clause(PHY_Address, devType, value);
}




#define REGCR   0x0D
#define ADDAR   0x0E

void MDIO_WriteRegisterClause22for45(uint32_t PHY_Address, uint16_t devad, uint32_t addr, uint32_t value)
{
    MDIO_WriteRegister(PHY_Address, REGCR, devad);
    MDIO_WriteRegister(PHY_Address, ADDAR, addr);
    MDIO_WriteRegister(PHY_Address, REGCR, 0x4000 | devad);
    MDIO_WriteRegister(PHY_Address, ADDAR, value);
}

uint16_t MDIO_ReadRegisterClause22for45(uint32_t PHY_Address, uint16_t devad, uint32_t addr)
{
    MDIO_WriteRegister(PHY_Address, REGCR, devad);
    MDIO_WriteRegister(PHY_Address, ADDAR, addr);
    MDIO_WriteRegister(PHY_Address, REGCR, 0x4000 | devad);
    return MDIO_ReadRegister(PHY_Address, ADDAR);
}

