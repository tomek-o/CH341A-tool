//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CSmartBatteryInfo.h"
#include "CH341A.h"
#include "SmartBattery.h"
#include "TabManager.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCH341I2CSmartBatteryInfo *frmCH341I2CSmartBatteryInfo;
//---------------------------------------------------------------------------

using namespace SmartBattery;

__fastcall TfrmCH341I2CSmartBatteryInfo::TfrmCH341I2CSmartBatteryInfo(TComponent* Owner)
	: TForm(Owner)
{
	for (unsigned int i=0; i<128; i++)
	{
		AnsiString text;
		text.sprintf("0x%02X (%3u)", i, i);
		cbI2CAddress->Items->Add(text);
	}
	cbI2CAddress->ItemIndex = SmartBattery::I2C_ADDRESS;

	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmCH341I2CSmartBatteryInfo::btnReadInfoClick(TObject *Sender)
{
	if (!ch341a.IsOpened())
	{
		MessageBox(this->Handle, "CH341 is not opened", this->Caption.c_str(), MB_ICONEXCLAMATION);
		return;
	}

	memoInfo->Clear();
	uint8_t address = static_cast<uint8_t>(cbI2CAddress->ItemIndex);

	int status;
	AnsiString text;
	int16_t data = 0;

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_BATTERY_MODE, data);
	if (status != 0)
	{
		text += "Error on CMD_BATTERY_MODE\r\n";
	}
	else
	{
		text += "BATTERY MODE:\r\n";
		text.cat_printf("    INTERNAL CHARGE CONTROLLER: %d\r\n", (data & MODE_INTERNAL_CHARGE_CONTROLLER)?1:0);
		text.cat_printf("    PRIMARY BATTERY SUPPORT: %d\r\n", (data & MODE_PRIMARY_BATTERY_SUPPORT)?1:0);
		text.cat_printf("    CONDITION REQUESTT: %d\r\n", (data & MODE_CONDITION_REQUEST)?1:0);
		text.cat_printf("    CHARGE CONTROLLER ENABLED: %d\r\n", (data & MODE_CHARGE_CONTROLLER_ENABLED)?1:0);
		text.cat_printf("    PRIMARY BATTERY: %d\r\n", (data & MODE_PRIMARY_BATTERY)?1:0);
		text.cat_printf("    ALARM broadcasts: %d\r\n", (data & MODE_ALARM)?1:0);
		text.cat_printf("    CHARGER broadcasts: %d\r\n", (data & MODE_CHARGER)?1:0);
		text.cat_printf("    CAPACITY: x%d\r mA/mAh\n", (data & MODE_CAPACITY)?10:1);
	}

	status = ch341a.I2CWriteCommandReadWord(address, CMD_BATTERY_STATUS, data);
	if (status != 0)
	{
		text += "Error on CMD_BATTERY_STATUS\r\n";
	}
	else
	{
		text += "BATTERY STATUS:\r\n";
		text.cat_printf("    OVERCHARGED ALARM = %d\r\n", (data & STATUS_OVER_CHARGED_ALARM)?1:0);
		text.cat_printf("    TERMINATE CHARGE ALARM = %d\r\n", (data & STATUS_TERMINATE_CHARGE_ALARM)?1:0);
		text.cat_printf("    OVER TEMP ALARM = %d\r\n", (data & STATUS_OVER_TEMP_ALARM)?1:0);
		text.cat_printf("    TERMINATE DISCHARGE ALARM = %d\r\n", (data & STATUS_TERMINATE_DISCHARGE_ALARM)?1:0);
		text.cat_printf("    REMAINING CAPACITY ALARM = %d\r\n", (data & STATUS_REMAINING_CAPACITY_ALARM)?1:0);
		text.cat_printf("    REMAINING TIME ALARM = %d\r\n", (data & STATUS_REMAINING_TIME_ALARM)?1:0);
		text.cat_printf("    INITIALIZED = %d\r\n", (data & STATUS_INITIALIZED)?1:0);
		text.cat_printf("    DISCHARGING = %d\r\n", (data & STATUS_DISCHARGING)?1:0);
		text.cat_printf("    FULLY CHARGED = %d\r\n", (data & STATUS_FULLY_CHARGED)?1:0);
		text.cat_printf("    FULLY DISCHARGED = %d\r\n", (data & STATUS_FULLY_DISCHARGED)?1:0);
	}

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_VOLTAGE, data);
	if (status != 0)
	{
		text += "Error on CMD_VOLTAGE\r\n";
	}
	else
	{
		text.cat_printf("Voltage: %d mV\r\n", static_cast<int>(data));
	}

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_MFG_DATE, data);
	if (status != 0)
	{
		text += "Error on CMD_MFG_DATE\r\n";
	}
	else
	{
		text.cat_printf("Mfg date: %d\r\n", static_cast<int>(data));
	}

	memoInfo->Text = text;
}
//---------------------------------------------------------------------------
