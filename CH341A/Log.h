/** \file
 *  \brief Global logging unit
 */

#ifndef LogH
#define LogH

#include <string>
#include "common/singleton.h"

/** \brief Global logger
*/
class CLog: public CSingleton<CLog>
{
public:
    /** \brief Log formatted text with timestamp */
	void log(char *lpData, ...);
	/** \brief Select output log file
	 *  \return zero on no error
	 */
	int SetFile(std::string file);
	void SetLogToFile(bool state);
	void SetTimestamps(bool state);
	void SetFlush(bool state);
	void SetMaxFileSize(unsigned int size);
	/** \brief Close log file */
	void Close(void);
	/** \brief OnLog callback declaration
	*/
	typedef void (__closure *CallbackLog)(char *buf);
	/** \brief Callback called when new log line is added
	 *
	 *  If set, this callback is called when new log line is added. This can
	 *  be used to update user interface (i.e. add some items to TListView)
	 *  when log is written by various decoupled units (with no knowledge
	 *  about user interface)     .
	 */
	CallbackLog callbackLog;
private:
	CLog();
	//~CLog() {};
	friend CSingleton<CLog>;
	std::string sFile;
	bool bLogToFile;
	bool addTimestamps;
	bool bFlush;
	unsigned int maxFileSize;
};

/** \brief Macro to avoid unnecessary typing
*/
#define LOG CLog::Instance()->log

#endif
