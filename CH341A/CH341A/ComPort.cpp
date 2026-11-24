//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ComPort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace
{
	/** Number after "COM", 0 if there is none */
	int PortNumber(const std::string &name)
	{
		if (name.size() > 3)
			return atoi(name.c_str() + 3);
		return 0;
	}

	bool PortBefore(const ComPort::PortInfo &a, const ComPort::PortInfo &b)
	{
		const int na = PortNumber(a.name);
		const int nb = PortNumber(b.name);
		if (na != nb)
			return na < nb;
		return a.name < b.name;
	}
}

int ComPort::EnumeratePorts(std::vector<PortInfo> &ports)
{
	ports.clear();

	HKEY key;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
		return -1;	// the key does not exist when there are no serial ports

	for (DWORD index = 0; ; index++)
	{
		char valueName[256];
		char data[256];
		DWORD nameSize = sizeof(valueName);
		DWORD dataSize = sizeof(data) - 1;
		DWORD type = 0;
		const LONG result = RegEnumValueA(key, index, valueName, &nameSize, NULL, &type, reinterpret_cast<LPBYTE>(data), &dataSize);
		if (result == ERROR_NO_MORE_ITEMS)
			break;
		if (result != ERROR_SUCCESS || type != REG_SZ)
			continue;
		data[dataSize] = 0;

		PortInfo port;
		port.name = data;
		// value name is the device, e.g. "\Device\USBSER000" or "\Device\Serial0"
		port.description = valueName;
		const std::string::size_type slash = port.description.rfind('\\');
		if (slash != std::string::npos)
			port.description = port.description.substr(slash + 1);
		ports.push_back(port);
	}
	RegCloseKey(key);

	// insertion sort (a few entries)
	for (size_t i = 1; i < ports.size(); i++)
	{
		const PortInfo entry = ports[i];
		size_t j = i;
		while (j > 0 && PortBefore(entry, ports[j - 1]))
		{
			ports[j] = ports[j - 1];
			j--;
		}
		ports[j] = entry;
	}
	return 0;
}

ComPort::ComPort(void):
	handle(INVALID_HANDLE_VALUE),
	lastError(0)
{
}

ComPort::~ComPort(void)
{
	Close();
}

int ComPort::Open(const std::string &portName, unsigned int baudrate)
{
	Close();

	// the \\.\ form is required for COM10 and above and works for all
	const std::string path = std::string("\\\\.\\") + portName;
	handle = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		lastError = GetLastError();
		return -1;
	}

	SetupComm(handle, 8192, 8192);

	DCB dcb;
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(handle, &dcb))
	{
		lastError = GetLastError();
		Close();
		return -2;
	}
	dcb.BaudRate = baudrate;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = 8;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fAbortOnError = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	if (!SetCommState(handle, &dcb))
	{
		lastError = GetLastError();
		Close();
		return -3;
	}

	// reads return immediately with whatever has been received
	COMMTIMEOUTS timeouts;
	memset(&timeouts, 0, sizeof(timeouts));
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 1000;
	SetCommTimeouts(handle, &timeouts);

	PurgeComm(handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	name = portName;
	lastError = 0;
	return 0;
}

void ComPort::Close(void)
{
	if (handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
	name.clear();
}

int ComPort::Read(void *data, unsigned int count)
{
	if (handle == INVALID_HANDLE_VALUE)
		return -1;
	DWORD done = 0;
	if (!ReadFile(handle, data, count, &done, NULL))
	{
		lastError = GetLastError();
		return -2;
	}
	return static_cast<int>(done);
}

int ComPort::Write(const void *data, unsigned int count)
{
	if (handle == INVALID_HANDLE_VALUE)
		return -1;
	DWORD done = 0;
	if (!WriteFile(handle, data, count, &done, NULL))
	{
		lastError = GetLastError();
		return -2;
	}
	if (done != count)
	{
		lastError = ERROR_TIMEOUT;
		return -3;
	}
	return 0;
}

std::string ComPort::GetLastErrorText(void) const
{
	char buf[256];
	const DWORD len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, sizeof(buf), NULL);
	std::string text(buf, len);
	while (!text.empty() && (text[text.size() - 1] == '\r' || text[text.size() - 1] == '\n' || text[text.size() - 1] == ' '))
		text.erase(text.size() - 1);
	char code[32];
	sprintf(code, " (error %lu)", static_cast<unsigned long>(lastError));
	return text + code;
}
