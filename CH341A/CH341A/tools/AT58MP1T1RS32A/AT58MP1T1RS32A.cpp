//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AT58MP1T1RS32A.h"
#include "ComPort.h"
#include "Log.h"
#include <string.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace
{
	const unsigned char HEADER = '$';
	const unsigned char END = '\r';
	enum { COMMAND_PAUSE_MS = 500 };	// the module needs time to execute a command
}

AT58MP1T1RS32A::AT58MP1T1RS32A(ComPort &port):
	port(port),
	frameBufferFill(0)
{
	memset(frameBuffer, 0, sizeof(frameBuffer));
}

unsigned int AT58MP1T1RS32A::addData(const unsigned char* data, unsigned int count)
{
	unsigned int newValuesCount = 0;

	for (unsigned int i=0; i<count; i++)
	{
		const unsigned char val = data[i];
		if (val == HEADER)
		{
			frameBufferFill = 0;
			frameBuffer[frameBufferFill++] = val;
		}
		else if (frameBufferFill < sizeof(frameBuffer) - 1)
		{
			frameBuffer[frameBufferFill++] = val;
			if (val == END)
			{
				if (frameBufferFill >= 8)
				{
					const unsigned char state = frameBuffer[7];
					if (state == '1')
					{
						valuesBuffer.push_back(1);
						newValuesCount++;
					}
					else if (state == '0')
					{
						valuesBuffer.push_back(0);
						newValuesCount++;
					}
				}
				frameBufferFill = 0;
			}
		}
	}

	return newValuesCount;
}

int AT58MP1T1RS32A::sendCommand(const std::string &cmd)
{
	if (!port.IsOpened())
		return -1;
	LOG("AT58: sending [%s]\n", cmd.c_str());
	if (port.Write(cmd.c_str(), static_cast<unsigned int>(cmd.size())) != 0)
		return -2;
	Sleep(COMMAND_PAUSE_MS);
	return 0;
}

int AT58MP1T1RS32A::setRange(unsigned int range)
{
	int err = 0;
	err |= sendCommand("sensorStop");
	char cmd[64];
	sprintf(cmd, "setRange 0 %u", range);
	err |= sendCommand(cmd);
	err |= sendCommand("saveConfig");	//Save configuration
	err |= sendCommand("sensorStart");	//Start the module to start running.
	return err;
}

int AT58MP1T1RS32A::setSensitivity(int sensitivity)
{
	int err = 0;
	err |= sendCommand("sensorStop");
	char cmd[64];
	sprintf(cmd, "setSensitivity %d", sensitivity);
	err |= sendCommand(cmd);
	err |= sendCommand("saveConfig");
	err |= sendCommand("sensorStart");
	return err;
}

int AT58MP1T1RS32A::setOutputLatency(int par1, int par2)
{
	int err = 0;
	err |= sendCommand("sensorStop");
	char cmd[64];
	sprintf(cmd, "setLatency %d %d", par1, par2);
	err |= sendCommand(cmd);
	err |= sendCommand("saveConfig");
	err |= sendCommand("sensorStart");
	return err;
}

int AT58MP1T1RS32A::factoryReset(void)
{
	int err = 0;
	err |= sendCommand("sensorStop");
	err |= sendCommand("resetCfg");
	err |= sendCommand("sensorStart");
	return err;
}

int AT58MP1T1RS32A::sensorStart(void)
{
	return sendCommand("sensorStart");
}
