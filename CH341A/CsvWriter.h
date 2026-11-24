//---------------------------------------------------------------------------

#ifndef CsvWriterH
#define CsvWriterH
//---------------------------------------------------------------------------

#include <stdio.h>
#include <System.hpp>
#include <vector>

/** Appends "time, value" lines to a new csv file (<exe dir>\csv\<date_time>_<suffix>.csv).
	The file is created when the first value arrives.
*/
class CsvWriter
{
private:
	FILE *fp;
	bool failed;
	AnsiString suffix;
public:
	/** \a fileSuffix: last part of the file name, "values" gives <date_time>_values.csv */
	explicit CsvWriter(const AnsiString &fileSuffix = "values");
	~CsvWriter(void);
	void AddValues(const std::vector<unsigned int> &values);
};

#endif
