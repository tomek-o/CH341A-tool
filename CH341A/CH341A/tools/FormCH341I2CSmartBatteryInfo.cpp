//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCH341I2CSmartBatteryInfo.h"
#include "CH341A.h"
#include "SmartBattery.h"
#include "TabManager.h"
#include "Log.h"
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

	// controller can get into sleep mode?
	for (unsigned int i=0; i<5; i++)
	{
		status = ch341a.I2CCheckDev(address);
		if (status == 0)
			break;
		Sleep(10);
	}

	if (status != 0)
	{
		memoInfo->Text = "No ACK from the I2C device / battery!";
		return;
	}

	Sleep(10);

	unsigned int capacityMultiplier = 1;
	const char *chargeUnit = "mA";
	const char *capacityUnit = "mAh";

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
		text.cat_printf("    CAPACITY mode: %s\r\n", (data & MODE_CAPACITY)?"x10 mW/mWh":"x1 mA/mAh");
		if (data & MODE_CAPACITY)
		{
			capacityMultiplier = 10;
			chargeUnit = "mW";
			capacityUnit = "mWh";
		}
	}

	(void)chargeUnit;

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

	{
		AnsiString str;
		status = ReadString(address, SmartBattery::CMD_MANUFACTURER_NAME, str);
		if (status != 0)
			text += "Error on CMD_MANUFACTURER_NAME\r\n";
		else
			text.cat_printf("Manufacturer name: %s\r\n", str.c_str());
	}

	{
		AnsiString str;
		status = ReadString(address, SmartBattery::CMD_DEVICE_NAME, str);
		if (status != 0)
			text += "Error on CMD_DEVICE_NAME\r\n";
		else
			text.cat_printf("Device name: %s\r\n", str.c_str());
	}

	{
		AnsiString str;
		status = ReadString(address, SmartBattery::CMD_CELL_CHEMISTRY, str);
		if (status != 0)
			text += "Error on CMD_CELL_CHEMISTRY\r\n";
		else
			text.cat_printf("Device chemistry: %s\r\n", str.c_str());
	}

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_MFG_DATE, data);
	if (status != 0)
	{
		text += "Error on CMD_MFG_DATE\r\n";
	}
	else
	{
		int day = data & 0x1F;
		int month = (data >> 5) & 0x0F;
		int year = 1980 + ((data >> 9) & 0x7F);
		text.cat_printf("Mfg date: %04d.%02d.%02d (raw value: %d)\r\n", year, month, day, static_cast<int>(data));
	}

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_SERIAL_NUM, data);
	if (status != 0)
		text += "Error on CMD_SERIAL_NUM\r\n";
	else
		text.cat_printf("Serial number: %u\r\n", static_cast<unsigned int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_VOLTAGE, data);
	if (status != 0)
	{
		text += "Error on CMD_VOLTAGE\r\n";
	}
	else
	{
		text.cat_printf("Voltage: %d mV\r\n", static_cast<int>(data));
	}

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_STATE_OF_HEALTH, data);
	if (status != 0)
		text += "Error on CMD_STATE_OF_HEALTH\r\n";
	else
		text.cat_printf("Health: %d%%\r\n", static_cast<int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_CHARGING_VOLTAGE, data);
	if (status != 0)
		text += "Error on CMD_CHARGING_VOLTAGE\r\n";
	else
		text.cat_printf("Charging voltage: %d mV\r\n", static_cast<int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_CHARGING_CURRENT, data);
	if (status != 0)
		text += "Error on CMD_CHARGING_CURRENT\r\n";
	else
		text.cat_printf("Charging current: %d mA\r\n", static_cast<int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_CYCLE_COUNT, data);
	if (status != 0)
		text += "Error on CMD_CYCLE_COUNT\r\n";
	else
		text.cat_printf("Cycle count: %u\r\n", static_cast<unsigned int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_REMAINING_CAPACITY, data);
	if (status != 0)
		text += "Error on CMD_REMAINING_CAPACITY\r\n";
	else
		text.cat_printf("Remaining capacity: %u %s\r\n", capacityMultiplier * static_cast<unsigned int>(data), capacityUnit);

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_RELATIVE_STATE_OF_CHARGE, data);
	if (status != 0)
		text += "Error on CMD_RELATIVE_STATE_OF_CHARGE\r\n";
	else
		text.cat_printf("Relative state of charge: %u%%\r\n", static_cast<unsigned int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_DESIGN_CAPACITY, data);
	if (status != 0)
		text += "Error on CMD_DESIGN_CAPACITY\r\n";
	else
		text.cat_printf("Design capacity: %u %s\r\n", capacityMultiplier * static_cast<unsigned int>(data), capacityUnit);

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_DESIGN_VOLTAGE, data);
	if (status != 0)
		text += "Error on CMD_DESIGN_VOLTAGE\r\n";
	else
		text.cat_printf("Design voltage: %u mV\r\n", static_cast<unsigned int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_FULL_CHARGE_CAPACITY, data);
	if (status != 0)
		text += "Error on CMD_FULL_CHARGE_CAPACITY\r\n";
	else
		text.cat_printf("Full charge capacity: %u %s\r\n", capacityMultiplier * static_cast<unsigned int>(data), capacityUnit);

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_CELL1_VOLTAGE, data);
	if (status != 0)
		text += "Error on CMD_CELL1_VOLTAGE\r\n";
	else
		text.cat_printf("Cell 1 voltage: %u mV\r\n", static_cast<unsigned int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_CELL2_VOLTAGE, data);
	if (status != 0)
		text += "Error on CMD_CELL2_VOLTAGE\r\n";
	else
		text.cat_printf("Cell 2 voltage: %u mV\r\n", static_cast<unsigned int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_CELL3_VOLTAGE, data);
	if (status != 0)
		text += "Error on CMD_CELL3_VOLTAGE\r\n";
	else
		text.cat_printf("Cell 3 voltage: %u mV\r\n", static_cast<unsigned int>(data));

	status = ch341a.I2CWriteCommandReadWord(address, SmartBattery::CMD_CELL4_VOLTAGE, data);
	if (status != 0)
		text += "Error on CMD_CELL4_VOLTAGE\r\n";
	else
		text.cat_printf("Cell 4 voltage: %u mV\r\n", static_cast<unsigned int>(data));


	memoInfo->Text = text;
}
//---------------------------------------------------------------------------

int TfrmCH341I2CSmartBatteryInfo::ReadString(uint8_t address, uint8_t command, AnsiString &str)
{
	int status;
	uint8_t len = 0;

	str = "";

	status = ch341a.I2CWriteCommandReadByte(address, command, len);
	if (status != 0)
	{
		return status;
	}

	//LOG("Smart battery string len = %u\n", static_cast<unsigned int>(len));
	//Sleep(1);

	uint8_t buffer[257] = {0};
	status = ch341a.I2CWriteCommandReadBuffer(address, command, buffer, len+1);	// read length (again) and data
	if (status != 0)
	{
		return status;
	}
	str = reinterpret_cast<char*>(buffer+1);	// +1: skip length
	return status;
}

