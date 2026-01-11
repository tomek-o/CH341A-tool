//---------------------------------------------------------------------------

#ifndef CH341AConfH
#define CH341AConfH
//---------------------------------------------------------------------------

#include <System.hpp>

namespace Json
{
	class Value;
}

struct CH341AConf
{
	bool openAtStartup;

	enum I2CSpeed {
		I2C_SPEED_20K = 0,
		I2C_SPEED_100K,
		I2C_SPEED_400K,
		I2C_SPEED_750K,
		I2C_SPEED__LIMITER
	} i2cSpeed;

	static const char* getI2CSpeedDescription(enum I2CSpeed val);

	bool sortToolsAlphabetically;
	AnsiString lastToolGroup;
	AnsiString lastPage;

	CH341AConf(void):
		openAtStartup(true),
		i2cSpeed(I2C_SPEED_20K),
		sortToolsAlphabetically(false)
	{}

	void fromJson(const Json::Value &jv);
	void toJson(Json::Value &jv) const;
};

#endif
