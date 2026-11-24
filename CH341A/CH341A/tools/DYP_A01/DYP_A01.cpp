//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DYP_A01.h"
#include "Log.h"
#include <string.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace
{
	const unsigned char HEADER = 0xFF;
	enum { MAX_BUFFERED_VALUES = 100000 };
	const unsigned int FRAME_BYTES = 4;	// = DYP_A01::FRAME_SIZE
}

DYP_A01::DYP_A01(void):
	frameBufferFill(0)
{
	memset(frameBuffer, 0, sizeof(frameBuffer));
}

unsigned int DYP_A01::AddData(const unsigned char* data, unsigned int count)
{
	unsigned int newValuesCount = 0;

	for (unsigned int i=0; i<count; i++)
	{
		const unsigned char val = data[i];
		if (frameBufferFill == 0)
		{
			if (val == HEADER)
				frameBuffer[frameBufferFill++] = val;
			continue;
		}

		frameBuffer[frameBufferFill++] = val;
		if (frameBufferFill < FRAME_BYTES)
			continue;

		// got a full frame
		const unsigned int sum = (frameBuffer[0] + frameBuffer[1] + frameBuffer[2]) & 0xFF;
		if (static_cast<unsigned int>(frameBuffer[FRAME_BYTES - 1]) == sum)
		{
			if (valuesBuffer.size() < MAX_BUFFERED_VALUES)
				valuesBuffer.push_back(256u * frameBuffer[1] + frameBuffer[2]);
			newValuesCount++;
			frameBufferFill = 0;
		}
		else
		{
			LOG("DYP-A01: invalid checksum, expected 0x%02X, received 0x%02X\n", sum,
				static_cast<unsigned int>(frameBuffer[FRAME_BYTES - 1]));
			// the next frame may have started inside this one: keep the bytes from its header on
			unsigned int pos = 1;
			while (pos < FRAME_BYTES && frameBuffer[pos] != HEADER)
				pos++;
			frameBufferFill = 0;
			for (; pos < FRAME_BYTES; pos++)
				frameBuffer[frameBufferFill++] = frameBuffer[pos];
		}
	}

	return newValuesCount;
}
