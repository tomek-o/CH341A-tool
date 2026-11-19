//---------------------------------------------------------------------------

#include <vcl.h>
#include "TabManager.h"
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("CH341A\FormCH341A.cpp", frmCH341A);
USEFORM("CH341A\tools\CH1115\FormCH341I2CCH1115.cpp", frmCH341I2CCH1115);
USEFORM("CH341A\tools\TEA5767\FormCH341I2CTea5767.cpp", frmCH341I2CTea5767);
USEFORM("CH341A\tools\SI4713\FormCH341_I2C_SI4713.cpp", frmCH341I2CSi4713);
USEFORM("CH341A\tools\BMI160\FormCH341I2CBMI160.cpp", frmCH341I2CBMI160);
USEFORM("CH341A\tools\MPU6050\FormCH341I2CMPU6050.cpp", frmCH341I2CMPU6050);
USEFORM("CH341A\tools\SHT11\FormCH341SHT11.cpp", frmCH341SHT11);
USEFORM("CH341A\tools\MDIO\FormCH341MdioDetect.cpp", frmCH341MdioDetect);
USEFORM("CH341A\tools\MDIO\FormCH341MdioWriteRead.cpp", frmCH341MdioWriteRead);
USEFORM("CH341A\tools\MDIO\FormCH341MdioMmdWriteRead.cpp", frmCH341MdioMmdWriteRead);
USEFORM("CH341A\tools\ST7567\FormCH341SpiST7567.cpp", frmCH341SpiST7567);
USEFORM("CH341A\tools\VEML7700\FormCH341_I2C_VEML7700.cpp", frmCH341I2CVeml7700);
USEFORM("CH341A\tools\TSL2591\FormCH341I2CTSL2591.cpp", frmCH341I2CTSL2591);
USEFORM("CH341A\tools\BH1750\FormCH341I2CBH1750.cpp", frmCH341I2CBH1750);
USEFORM("CH341A\tools\KT0803L\FormKT0803L.cpp", frmKT0803L);
USEFORM("CH341A\tools\MPR121\FormMPR121.cpp", frmMPR121);
USEFORM("CH341A\tools\TM1637\FormCH341_TM1637.cpp", frmCH341Tm1637);
USEFORM("CH341A\tools\MAX30102\FormCH341I2CMAX30102.cpp", frmCH341I2CMAX30102);
USEFORM("CH341A\tools\PN532\FormCH341_SPI_PN532.cpp", frmCH341_SPI_PN532);
USEFORM("CH341A\tools\NRF24L01\FormCH341SpiNrf24L01TxRx.cpp", frmCH341SpiNrf24L01TxRx);
USEFORM("CH341A\tools\MemProg\FormCH341MemProg.cpp", frmCH341MemProg);
USEFORM("CH341A\tools\SdCard\FormCH341SdCard.cpp", frmCH341SdCard);
USEFORM("CH341A\tools\SdCard\FormCH341SdData.cpp", frmCH341SdData);
USEFORM("CH341A\tools\FormCH341Gpio.cpp", frmCH341Gpio);
USEFORM("CH341A\tools\FormCH341GpioPatternGenerator.cpp", frmCH341GpioPatternGenerator);
USEFORM("CH341A\tools\FormCH341I2CDetect.cpp", frmCH341I2CDetect);
USEFORM("CH341A\tools\FormCH341I2CSi7021.cpp", frmCH341I2CSi7021);
USEFORM("CH341A\tools\FormCH341I2CSHT4x.cpp", frmCH341I2CSHT4x);
USEFORM("CH341A\tools\FormCH341I2CAHT20.cpp", frmCH341I2CAHT20);
USEFORM("CH341A\tools\FormCH341SpiWriteRead.cpp", frmCH341SpiWriteRead);
USEFORM("CH341A\tools\FormCH341I2CSmartBatteryInfo.cpp", frmCH341I2CSmartBatteryInfo);
USEFORM("FormMain.cpp", frmMain);
USEFORM("FormSettings.cpp", frmSettings);
USEFORM("FormAbout.cpp", frmAbout);
USEFORM("FrameHexEditor.cpp", frameHexEditor); /* TFrame: File Type */
USEFORM("LogUnit.cpp", frmLog);
USEFORM("CH341A\tools\FormCH341I2CSsd1306.cpp", frmCH341I2CSsd1306);
USEFORM("CH341A\tools\FormCH341I2CBme280.cpp", frmCH341I2CBme280);
USEFORM("CH341A\tools\FormCH341I2CBmp180.cpp", frmCH341I2CBmp180);
USEFORM("CH341A\tools\FormCH341I2CLm75.cpp", frmCH341I2CLm75);
USEFORM("CH341A\tools\FormCH341I2CAs5600.cpp", frmCH341I2CAs5600);
USEFORM("CH341A\tools\FormCH341GpioHd44780.cpp", frmCH341GpioHd44780);
USEFORM("CH341A\tools\FormCH341I2CHd44780.cpp", frmCH341I2CHd44780);
USEFORM("CH341A\tools\FormCH341I2CSi5351.cpp", frmCH341I2CSi5351);
USEFORM("CH341A\tools\FormCH341I2CWriteRead.cpp", frmCH341I2CWriteRead);
USEFORM("CH341A\tools\MAX7219\FormCH341SpiMax7219Led8x8.cpp", frmCH341SpiMax7219Led8x8);
USEFORM("CH341A\tools\MAX7219\FormCH341SpiMax7219Led8x8Cascade.cpp", frmCH341SpiMax7219Led8x8Cascade);
USEFORM("CH341A\tools\FormCH341I2CVl53l0x.cpp", frmCH341I2CVl53l0x);
USEFORM("CH341A\tools\FormCH341I2CApds9960.cpp", frmCH341I2CApds9960);
USEFORM("CH341A\tools\FormCH341I2CMCP4725.cpp", frmCH341I2CMCP4725);
USEFORM("CH341A\tools\RDA5807M\FormCH341I2CRda5807m.cpp", frmCH341I2CRda5807m);
USEFORM("CH341A\tools\FormCH341I2CL3G4200D.cpp", frmCH341I2CL3G4200D);
USEFORM("FormPlot.cpp", frmPlot);
USEFORM("CH341A\tools\MDIO\FormCH341MdioLan8742Tdr.cpp", frmCH341MdioLan8742Tdr);
USEFORM("CH341A\tools\MDIO\FormCH341MdioLan8742Cbln.cpp", frmCH341MdioLan8742Cbln);
USEFORM("CH341A\tools\FormCH341DS1302.cpp", frmCH341DS1302);
USEFORM("CH341A\tools\FormCH341DS1307.cpp", frmCH341DS1307);
USEFORM("CH341A\tools\FormCH341DS3231.cpp", frmCH341DS3231);
USEFORM("CH341A\tools\FormCH341_I2C_S35390A.cpp", frmCH341_I2C_S35390A);
USEFORM("CH341A\tools\FormCH341I2CCH455.cpp", frmCH341I2CCH455);
USEFORM("CH341A\tools\FormCH341I2CAds1115.cpp", frmCH341I2CAds1115);
USEFORM("CH341A\tools\FormCH341I2CMax11601.cpp", frmCH341I2CMax11601);
USEFORM("CH341A\tools\FormCH341Hmc5883L.cpp", frmCH341Hmc5883L);
USEFORM("CH341A\tools\FormCH341Ina226.cpp", frmCH341Ina226);
USEFORM("CH341A\tools\MDIO\FormCH341USB2MDIO.cpp", frmCH341USB2MDIO);
USEFORM("CH341A\tools\FormCH341Max6675.cpp", frmCH341Max6675);
USEFORM("CH341A\tools\PN532\FormCH341PN532History.cpp", frmCH341PN532History);
//---------------------------------------------------------------------------
#pragma link "common.lib"
#pragma link "jsoncpp.lib"
#pragma link "CH341DLL.lib"

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "CH341 tool";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmSettings), &frmSettings);
		Application->CreateForm(__classid(TfrmAbout), &frmAbout);
		Application->CreateForm(__classid(TfrmLog), &frmLog);
		TabManager::Instance().LogStartupTime();
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
