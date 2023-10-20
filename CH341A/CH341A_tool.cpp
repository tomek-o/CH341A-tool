//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("CH341A\FormCH341A.cpp", frmCH341A);
USEFORM("CH341A\tools\FormCH341Gpio.cpp", frmCH341Gpio);
USEFORM("CH341A\tools\FormCH341I2CDetect.cpp", frmCH341I2CDetect);
USEFORM("CH341A\tools\FormCH341I2CWriteRead.cpp", frmCH341I2CWriteRead);
USEFORM("CH341A\tools\FormCH341I2CSmartBatteryInfo.cpp", frmCH341I2CSmartBatteryInfo);
USEFORM("FormMain.cpp", frmMain);
USEFORM("FormSettings.cpp", frmSettings);
USEFORM("FormAbout.cpp", frmAbout);
USEFORM("LogUnit.cpp", frmLog);
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
		Application->CreateForm(__classid(TfrmCH341I2CSmartBatteryInfo), &frmCH341I2CSmartBatteryInfo);
		Application->CreateForm(__classid(TfrmCH341Gpio), &frmCH341Gpio);
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
