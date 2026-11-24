//---------------------------------------------------------------------------
/** \file
	DYP-A01 ultrasonic distance sensor, UART output.
	The module sends 4-byte frames: 0xFF, distance high byte, distance low byte, checksum
	(low 8 bits of the sum of the first three bytes). Distance is in millimeters.
*/

#ifndef DYP_A01H
#define DYP_A01H
//---------------------------------------------------------------------------

#include <vector>

class DYP_A01
{
private:
	enum { FRAME_SIZE = 4 };
	std::vector<unsigned int> valuesBuffer;
	unsigned char frameBuffer[FRAME_SIZE];
	unsigned int frameBufferFill;
public:
	DYP_A01(void);
	/** Feeds received bytes
		\return Number of new values
	*/
	unsigned int AddData(const unsigned char* data, unsigned int count);
	/** Takes the distances (mm) decoded so far */
	void GetValues(std::vector<unsigned int> &values) {
		values = valuesBuffer;
		valuesBuffer.clear();
	}
};

#endif
