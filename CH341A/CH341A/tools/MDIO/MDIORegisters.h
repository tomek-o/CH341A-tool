//---------------------------------------------------------------------------

#ifndef MDIORegistersH
#define MDIORegistersH
//---------------------------------------------------------------------------

#include <System.hpp>

enum MdioRegister
{
	MDIO_REG_BASIC_CONTROL = 0,
	MDIO_REG_BASIC_STATUS,
	MDIO_REG_PHY_ID_1,
	MDIO_REG_PHY_ID_2,
	MDIO_REG_AUTONEG_ADVERTISEMENT,
	MDIO_REG_AUTONEG_LINK_PARTNER_ABILITY,
	MDIO_REG_AUTONEG_EXPANSION,
	MDIO_REG_AUTONEG_NEXT_PAGE_TX,
	MDIO_REG_AUTONEG_NEXT_PAGE_RX,

	MDIO_REG_MMD_ACCESS_CONTROL = 13,
	MDIO_REG_MMD_ACCESS_ADDRESS_DATA = 14,
};

const char* MdioRegisterName(enum MdioRegister reg);

struct MdioRegDescription
{
    enum MdioRegister reg;
	bool rFlag;
	bool wFlag;
	AnsiString caption;
	AnsiString bitCaption[16];
};

void MdioGetRegisterDescriptions(const struct MdioRegDescription* &regs, unsigned int &count);


#endif
