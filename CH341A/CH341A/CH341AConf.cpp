//---------------------------------------------------------------------------


#pragma hdrstop

#include "CH341AConf.h"
#include <json/json.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)


const char* CH341AConf::getI2CSpeedDescription(enum I2CSpeed val)
{
	switch (val)
	{
	case I2C_SPEED_20K:
		return "low / 20 kHz";
	case I2C_SPEED_100K:
		return "standard / 100 kHz";
	case I2C_SPEED_400K:
		return "fast / 400 kHz";
	case I2C_SPEED_750K:
		return "high / 750 kHz";
	default:
		return "???";
	}
}

void CH341AConf::fromJson(const Json::Value &jv)
{
	if (jv.type() != Json::objectValue)
		return;
	int tmp = i2cSpeed;
	jv.getInt("i2cSpeed", tmp);
	if (tmp >= 0 && tmp < I2C_SPEED__LIMITER)
	{
    	i2cSpeed = static_cast<I2CSpeed>(tmp);
	}
}

void CH341AConf::toJson(Json::Value &jv) const
{
	jv = Json::Value(Json::objectValue);
	jv["i2cSpeed"] = i2cSpeed;
}
