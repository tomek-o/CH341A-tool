//---------------------------------------------------------------------------


#pragma hdrstop

#include "MDIORegisters.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

namespace
{

const struct MdioRegDescription regDescriptions[] = {
	{MDIO_REG_BASIC_CONTROL, true,	true,	"Basic Control Register",
		{
			"RESERVED", "RESERVED", "RESERVED", "RESERVED",
			"RESERVED", "RESERVED", "RESERVED", "RESERVED",
			"Duplex Mode (1 = full)", "Restart Auto-Negotiate", "Isolate", "Power Down",
			"Auto-Negotiation Enable", "Speed Select", "Loopback", "Soft Reset",
		}
	},
	{MDIO_REG_BASIC_STATUS, true, false,	"Basic Status Register",
		{
			"Extended Capabilities", "Jabber Detect", "Link Status", "Auto-Negotiate Ability",
			"Remote Fault", "Auto-Negotiate Complete", "RESERVED", "RESERVED",
			"Extended Status", "100BASE-T2 Half Duplex", "100BASE-T2 Full Duplex", "10BASE-T Half Duplex",
			"10BASE-T Full Duplex", "100BASE-TX Half Duplex", "100BASE-TX Full Duplex", "100BASE-T4",
		}
	},
	{MDIO_REG_PHY_ID_1, true, true,			"PHY Identifier 1",
		{
			"", "", "", "",
			"", "", "", "",
			"", "", "", "",
			"", "", "", ""
		}
	},
	{MDIO_REG_PHY_ID_2, true, true,			"PHY Identifier 2",
		{
			"", "", "", "",
			"", "", "", "",
			"", "", "", "",
			"", "", "", ""
		}
	},
	{MDIO_REG_AUTONEG_ADVERTISEMENT, true, true,	"Auto Negotiation Advertisement",
		{
			"Selector Field", "Selector Field", "Selector Field", "Selector Field",
			"Selector Field", "10BASE-T", "10BASE-T Full Duplex", "100BASE-TX",
			"100BASE-TX Full Duplex", "RESERVED", "Pause Operation bit 0", "Pause Operation bit 1",
			"RESERVED", "Remote Fault", "RESERVED", "Next Page",
		}
	},
	{MDIO_REG_AUTONEG_LINK_PARTNER_ABILITY, true, false,	"Auto Negotiation Link Partner Ability",
		{
			"Selector Field", "Selector Field", "Selector Field", "Selector Field",
			"Selector Field", "10BASE-T", "10BASE-T Full Duplex", "100BASE-TX",
			"100BASE-TX Full Duplex", "100BASE-T4", "Pause Operation bit 0", "Pause Operation bit 1",
			"RESERVED", "Remote Fault", "Acknowledge", "Next Page",
		}
	},
	{MDIO_REG_AUTONEG_EXPANSION, true, false, "Auto Negotiation Expansion",
		{
			"Link Partner Auto-Negotiation Able", "Page Received", "Next Page Able", "Link Partner Next Page Able",
			"Parallel Detection Fault", "Received Next Page Storage Location", "Receive Next Page Location Able", "RESERVED",
			"RESERVED", "RESERVED", "RESERVED", "RESERVED",
			"RESERVED", "RESERVED", "RESERVED", "RESERVED",
		}
	},
	{MDIO_REG_AUTONEG_NEXT_PAGE_TX, true, true, "Auto Negotiation Next Page Tx",
		{
			"Message Code", "Message Code", "Message Code", "Message Code",
			"Message Code", "Message Code", "Message Code", "Message Code",
			"Message Code", "Message Code", "Message Code", "Toggle",
			"Acknowledge 2", "Message Page", "RESERVED", "Next Page",
		}
	},
	{MDIO_REG_AUTONEG_NEXT_PAGE_RX, true, false, "Auto Negotiation Next Page Rx",
		{
			"Message Code", "Message Code", "Message Code", "Message Code",
			"Message Code", "Message Code", "Message Code", "Message Code",
			"Message Code", "Message Code", "Message Code", "Toggle",
			"Acknowledge 2", "Message Page", "Acknowledge", "Next Page",
		}
	},
	{MDIO_REG_MMD_ACCESS_CONTROL, true, true, "MMD Access Control",
		{
			"", "", "", "",
			"", "", "", "",
			"", "", "", "",
			"", "", "", ""
		}
	},
	{MDIO_REG_MMD_ACCESS_ADDRESS_DATA, true, true, "MMD Access Address/Data",
		{
			"", "", "", "",
			"", "", "", "",
			"", "", "", "",
			"", "", "", ""		
		}
	}
};

}


void MdioGetRegisterDescriptions(const struct MdioRegDescription* &regs, unsigned int &count)
{
	regs = regDescriptions;
	count = ARRAY_SIZE(regDescriptions);
}

