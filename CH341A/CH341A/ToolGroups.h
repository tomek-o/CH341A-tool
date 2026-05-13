//---------------------------------------------------------------------------

#ifndef ToolGroupsH
#define ToolGroupsH
//---------------------------------------------------------------------------

enum ToolGroup
{
	ToolGroupAll = 0,
	ToolGroupGenericInterfaces,
	ToolGroupSmartBattery,
	ToolGroupSensors,
	ToolGroupDisplays,
	ToolGroupFmRadio,
	//ToolGroupRadio,
	ToolGroupEthernetMdio,
	ToolGroupRtc,
	ToolGroupAdc,
	ToolGroupDac,
	ToolGroupKeypad,
	ToolGroupMisc,

	ToolGroup_Limiter
};

const char* GetToolGroupName(enum ToolGroup group);

#endif
