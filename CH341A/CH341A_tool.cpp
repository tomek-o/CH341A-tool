//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("CH341A\FormCH341A.cpp", frmCH341A);
USEFORM("CH341A\tools\FormCH341Gpio.cpp", frmCH341Gpio);
USEFORM("CH341A\tools\FormCH341I2CDetect.cpp", frmCH341I2CDetect);
USEFORM("CH341A\tools\FormCH341I2CSi7021.cpp", frmCH341I2CSi7021);
USEFORM("CH341A\tools\FormCH341I2CAHT20.cpp", frmCH341I2CAHT20);
USEFORM("CH341A\tools\FormCH341SpiWriteRead.cpp", frmCH341SpiWriteRead);
USEFORM("CH341A\tools\FormCH341I2CSmartBatteryInfo.cpp", frmCH341I2CSmartBatteryInfo);
USEFORM("FormMain.cpp", frmMain);
USEFORM("FormSettings.cpp", frmSettings);
USEFORM("FormAbout.cpp", frmAbout);
USEFORM("LogUnit.cpp", frmLog);
USEFORM("CH341A\tools\FormCH341I2CSsd1306.cpp", frmCH341I2CSsd1306);
USEFORM("CH341A\tools\FormCH341I2CRda5807m.cpp", frmCH341I2CRda5807m);
USEFORM("CH341A\tools\FormCH341I2CLm75.cpp", frmCH341I2CLm75);
USEFORM("CH341A\tools\FormCH341I2CBme280.cpp", frmCH341I2CBme280);
USEFORM("CH341A\tools\FormCH341GpioHd44780.cpp", frmCH341GpioHd44780);
USEFORM("CH341A\tools\FormCH341I2CHd44780.cpp", frmCH341I2CHd44780);
USEFORM("CH341A\tools\FormCH341I2CBmp180.cpp", frmCH341I2CBmp180);
USEFORM("CH341A\tools\FormCH341I2CWriteRead.cpp", frmCH341I2CWriteRead);
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
		Application->CreateForm(__classid(TfrmCH341I2CDetect), &frmCH341I2CDetect);
		Application->CreateForm(__classid(TfrmCH341I2CWriteRead), &frmCH341I2CWriteRead);
		Application->CreateForm(__classid(TfrmCH341SpiWriteRead), &frmCH341SpiWriteRead);
		Application->CreateForm(__classid(TfrmCH341Gpio), &frmCH341Gpio);
		Application->CreateForm(__classid(TfrmCH341I2CSmartBatteryInfo), &frmCH341I2CSmartBatteryInfo);
		Application->CreateForm(__classid(TfrmCH341I2CSi7021), &frmCH341I2CSi7021);
		Application->CreateForm(__classid(TfrmCH341I2CBme280), &frmCH341I2CBme280);
		Application->CreateForm(__classid(TfrmCH341I2CBmp180), &frmCH341I2CBmp180);
		Application->CreateForm(__classid(TfrmCH341I2CLm75), &frmCH341I2CLm75);
		Application->CreateForm(__classid(TfrmCH341I2CAHT20), &frmCH341I2CAHT20);
		Application->CreateForm(__classid(TfrmCH341I2CSsd1306), &frmCH341I2CSsd1306);
		Application->CreateForm(__classid(TfrmCH341GpioHd44780), &frmCH341GpioHd44780);
		Application->CreateForm(__classid(TfrmCH341I2CHd44780), &frmCH341I2CHd44780);
		Application->CreateForm(__classid(TfrmCH341I2CRda5807m), &frmCH341I2CRda5807m);
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
