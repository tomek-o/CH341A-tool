//---------------------------------------------------------------------------
/** \file
	\brief Serial port (UART) access, WinAPI, non-blocking reads
*/

#ifndef ComPortH
#define ComPortH
//---------------------------------------------------------------------------

#include <windows.h>
#include <string>
#include <vector>

class ComPort
{
public:
	struct PortInfo
	{
		std::string name;		///< "COM5"
		std::string description;	///< driver / device the port belongs to, e.g. "USBSER000" (USB serial), may be empty
	};

	/** Lists the serial ports present in the system, sorted by number
		\return 0 on success
	*/
	static int EnumeratePorts(std::vector<PortInfo> &ports);

	ComPort(void);
	~ComPort(void);

	/** Opens \a name ("COM5", also works for numbers above 9) as 8N1 without flow control
		\return 0 on success, see GetLastErrorText() otherwise
	*/
	int Open(const std::string &name, unsigned int baudrate);
	void Close(void);
	bool IsOpened(void) const { return handle != INVALID_HANDLE_VALUE; }
	const std::string& GetName(void) const { return name; }

	/** Reads what is available now, does not wait
		\return number of bytes read (0 if nothing is waiting) or negative on error
	*/
	int Read(void *data, unsigned int count);
	/** Writes all \a count bytes
		\return 0 on success
	*/
	int Write(const void *data, unsigned int count);

	/** Text of the last Windows error of Open / Read / Write */
	std::string GetLastErrorText(void) const;

private:
	HANDLE handle;
	std::string name;
	DWORD lastError;
	ComPort(const ComPort&);
	ComPort& operator=(const ComPort&);
};

//---------------------------------------------------------------------------
#endif
