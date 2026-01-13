#ifndef MDIOH
#define MDIOH

#include <stdint.h>

int MDIO_Init(void);

void MDIO_WriteRegister(uint32_t PHY_Address, uint32_t PHY_Register, uint32_t value);
uint16_t MDIO_ReadRegister(uint32_t PHY_Address, uint32_t PHY_Register);

void MDIO_WriteRegisterClause45(uint32_t PHY_Address, uint8_t devType, uint32_t addr, uint32_t value);
uint32_t MDIO_ReadRegisterClause45(uint32_t PHY_Address, uint8_t devType, uint32_t addr);


void MDIO_WriteRegisterClause22for45(uint32_t PHY_Address, uint16_t devad, uint32_t addr, uint32_t value);
uint16_t MDIO_ReadRegisterClause22for45(uint32_t PHY_Address, uint16_t devad, uint32_t addr);

/** \brief Read function for Texas Instruments USB-2-MDIO */
uint16_t MDIO_TI_ExtendedRead(uint32_t phyAddress, uint32_t phyRegister);
/** \brief Write function for Texas Instruments USB-2-MDIO */
void MDIO_TI_ExtendedWrite(uint32_t phyAddress, uint32_t phyRegister, uint16_t value);

#endif
