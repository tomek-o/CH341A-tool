//---------------------------------------------------------------------------
/** \file
	AT58MP1T1RS32A 5.8 GHz microwave motion sensor, UART interface.
	The module sends frames "$..." terminated with CR, the 8th character is the detection state
	('1' = target present, '0' = none). Configuration is done with text commands.
*/

#ifndef AT58MP1T1RS32AH
#define AT58MP1T1RS32AH
//---------------------------------------------------------------------------

#include <vector>
#include <string>

class ComPort;

class AT58MP1T1RS32A
{
private:
	ComPort &port;
	std::vector<unsigned int> valuesBuffer;
	unsigned char frameBuffer[64];
	unsigned int frameBufferFill;
	int sendCommand(const std::string &cmd);
public:
	explicit AT58MP1T1RS32A(ComPort &port);
	/** Feeds received bytes
		\return Number of new values
	*/
	unsigned int addData(const unsigned char* data, unsigned int count);
	/** Takes the values decoded so far (0 / 1) */
	void getValues(std::vector<unsigned int> &values) {
		values = valuesBuffer;
		valuesBuffer.clear();
	}

	/** \param range range in meters, 0...11, default: 6 meters
	*/
	int setRange(unsigned int range);

	/**
	   @brief Configure detection sensitivity
	   @param sensitivity Detection sensitivity, the greater the value from 0 to 9, the higher the sensitivity, and the default value is 7.
	*/
	int setSensitivity(int sensitivity);

	/**
	   @brief Configure output delay time
	   @param par1 Confirm delay; When the target is detected, the target valid signal will be output after the par1 duration. The value range is 0 ~ 100, unit second, and the default value is 1 second
				   For scenes where the detection response speed is not high, it is recommended that it be no less than 1 second, which can greatly reduce the false alarm rate.
	   @param par2 Disappearing delay; If the target is not detected, an invalid target signal will be output after the par2 duration. The value range is 1 ~ 1500, and the unit second is 15 seconds by default.
	*/
	int setOutputLatency(int par1, int par2);

	/**
	   @brief Restore factory settings
	*/
	int factoryReset(void);

	int sensorStart(void);
};

#endif
