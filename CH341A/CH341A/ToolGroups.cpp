//---------------------------------------------------------------------------


#pragma hdrstop

#include "ToolGroups.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

const char* GetToolGroupName(enum ToolGroup group)
{
	switch (group)
	{
	case ToolGroupAll:
		return "All tools";
	case ToolGroupGenericInterfaces:
		return "Generic interfaces";
	case ToolGroupSmartBattery:
		return "Smart battery";
	case ToolGroupSensors:
		return "Sensors";
	case ToolGroupDisplays:
		return "Displays";
	case ToolGroupFmRadio:
		return "FM radio";
	case ToolGroupEthernetMdio:
		return "Ethernet MDIO";
	case ToolGroupRtc:
		return "RTC";
	case ToolGroupAdc:
		return "ADC";
	case ToolGroupDac:
		return "DAC";
	case ToolGroupMisc:
		return "Miscellaneous";
	default:
		return "???";
	}
}
