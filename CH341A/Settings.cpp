//---------------------------------------------------------------------------


#pragma hdrstop

#include "Settings.h"
#include <algorithm>
#include <fstream>
#include <json/json.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

Settings appSettings;

inline void strncpyz(char* dst, const char* src, int dstsize) {
	strncpy(dst, src, dstsize);
	dst[dstsize-1] = '\0';
}

void Settings::SetDefault(void)
{
	frmMain.iWidth = 350;
	frmMain.iHeight = 300;
	frmMain.iPosX = 30;
	frmMain.iPosY = 30;
	frmMain.bWindowMaximized = false;
	frmMain.bAlwaysOnTop = false;

	Logging.bLogToFile = false;
	Logging.bFlush = false;
	Logging.iMaxFileSize = Settings::_Logging::DEF_MAX_FILE_SIZE;
	Logging.iMaxUiLogLines = 5000;
}

int Settings::Read(AnsiString asFileName)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

    SetDefault();

	try
	{
		std::ifstream ifs(asFileName.c_str());
		std::string strConfig((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		bool parsingSuccessful = reader.parse( strConfig, root );
		if ( !parsingSuccessful )
		{
			return 2;
		}
	}
	catch(...)
	{
		return 1;
	}

	int maxX = GetSystemMetrics(SM_CXSCREEN);
	/** \todo Ugly fixed taskbar margin */
	int maxY = GetSystemMetrics(SM_CYSCREEN) - 32;

	const Json::Value &frmMainJson = root["frmMain"];
	frmMain.iWidth = frmMainJson.get("AppWidth", 350).asInt();
	frmMain.iHeight = frmMainJson.get("AppHeight", 300).asInt();
	if (frmMain.iWidth < 250 || frmMain.iWidth > maxX + 20)
	{
		frmMain.iWidth = 250;
	}
	if (frmMain.iHeight < 200 || frmMain.iHeight > maxY + 20)
	{
		frmMain.iHeight = 200;
	}
	frmMain.iPosX = frmMainJson.get("AppPositionX", 30).asInt();
	frmMain.iPosY = frmMainJson.get("AppPositionY", 30).asInt();
	if (frmMain.iPosX < 0)
		frmMain.iPosX = 0;
	if (frmMain.iPosY < 0)
		frmMain.iPosY = 0;
	if (frmMain.iPosX + frmMain.iWidth > maxX)
		frmMain.iPosX = maxX - frmMain.iWidth;
	if (frmMain.iPosY + frmMain.iHeight > maxY)
		frmMain.iPosY = maxY - frmMain.iHeight;
	frmMain.bWindowMaximized = frmMainJson.get("Maximized", false).asBool();
	frmMain.bAlwaysOnTop = frmMainJson.get("AlwaysOnTop", false).asBool();

	const Json::Value &LoggingJson = root["Logging"];
	Logging.bLogToFile = LoggingJson.get("LogToFile", false).asBool();
	Logging.bFlush = LoggingJson.get("Flush", Logging.bFlush).asBool();
	Logging.iMaxFileSize = LoggingJson.get("MaxFileSize", Logging.iMaxFileSize).asInt();
	if (Logging.iMaxFileSize < Settings::_Logging::MIN_MAX_FILE_SIZE || Logging.iMaxFileSize > Settings::_Logging::MIN_MAX_FILE_SIZE)
	{
		Logging.iMaxFileSize = Settings::_Logging::DEF_MAX_FILE_SIZE;
	}
	Logging.iMaxUiLogLines = LoggingJson.get("MaxUiLogLines", 5000).asInt();

	return 0;
}

int Settings::Write(AnsiString asFileName)
{
	Json::Value root;
	Json::StyledWriter writer;

	root["frmMain"]["AppWidth"] = frmMain.iWidth;
	root["frmMain"]["AppHeight"] = frmMain.iHeight;
	root["frmMain"]["AppPositionX"] = frmMain.iPosX;
	root["frmMain"]["AppPositionY"] = frmMain.iPosY;
	root["frmMain"]["Maximized"] = frmMain.bWindowMaximized;
	root["frmMain"]["AlwaysOnTop"] = frmMain.bAlwaysOnTop;

	root["Logging"]["LogToFile"] = Logging.bLogToFile;
	root["Logging"]["Flush"] = Logging.bFlush;
	root["Logging"]["MaxFileSize"] = Logging.iMaxFileSize;
	root["Logging"]["MaxUiLogLines"] = Logging.iMaxUiLogLines;


	std::string outputConfig = writer.write( root );

	try
	{
		std::ofstream ofs(asFileName.c_str());
		ofs << outputConfig;
		ofs.close();
	}
	catch(...)
	{
    	return 1;
	}

	return 0;
}


