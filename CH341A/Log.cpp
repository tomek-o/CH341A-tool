/** \file
 *  \brief Global logging unit
 */

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <time.h>
#include <sys\timeb.h>
#include "Log.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "common/Mutex.h"
#include "common/ScopedLock.h"

namespace {
	FILE *fout = NULL;
	Mutex mutex;
}

CLog::CLog()
{
	sFile = "";
	iLogLevel = 0;
	bLogToFile = true;
	bFlush = false;
	callbackLog = NULL;
	maxFileSize = 0;
};

int CLog::SetFile(std::string file)
{
	ScopedLock<Mutex> lock(mutex);
	sFile = file;
	iLogLevel = E_LOG_NONE;
	if (fout)
	{
		fclose(fout);
		fout = NULL;
	}
	if (file == "")
		return -1;
	fout = fopen(sFile.c_str(),"at+");
	if (fout == NULL)
	{
		sFile = "";	
		return -1;
	}
	return 0;
}

void CLog::Close(void)
{
	ScopedLock<Mutex> lock(mutex);
	if (fout)
		fclose(fout);
	fout = NULL;
}

void CLog::SetLogToFile(bool state)
{
	bLogToFile = state;
}

void CLog::SetFlush(bool state)
{
    bFlush = state;
}

void CLog::SetMaxFileSize(unsigned int size)
{
    maxFileSize = size;
}

void CLog::SetLevel(int level)
{
	iLogLevel = level;
}

void CLog::log(char *lpData, ...)
{
	ScopedLock<Mutex> lock(mutex);
	va_list ap;
	char buf[2048]; //determines max message length

	/*
	After looking inside RTL sources it seems that this is thread-safe (when linking
	with MT version).
	*/
	//int size = strftime(buf, sizeof(buf), "%Y-%m-%d %T", localtime(&timebuffer.time));
#if 0
	struct timeb timebuffer;
	ftime( &timebuffer );
	int size = strftime(buf, sizeof(buf), "%T", localtime(&timebuffer.time));
	int res = snprintf(buf+size, sizeof(buf)-size, ".%03hu ", timebuffer.millitm);
	buf[sizeof(buf)-1] = '\0';
	size += res;
#else
	int size = 0;
#endif

	if ((int)sizeof(buf)-size-2 > 0)
	{
		va_start(ap, lpData);
		size += vsnprintf(buf + size, sizeof(buf)-size-2, lpData, ap);
		va_end(ap);
	}
	if (size > (int)sizeof(buf) - 2)
		size = sizeof(buf) - 2;

	buf[size] = '\0';

	if (bLogToFile && fout)
	{
		fwrite(buf, size, 1, fout);
		if (bFlush)
		{
            fflush(fout);
		}
		if (maxFileSize != 0)
		{
			unsigned int size = ftell(fout);
			if (size > maxFileSize)
			{
				// truncate
				fclose(fout);
				fout = fopen(sFile.c_str(),"wt+");								
            }
        }
	}

	if (callbackLog)
		callbackLog(buf);
}

