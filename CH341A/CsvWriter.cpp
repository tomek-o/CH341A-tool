//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CsvWriter.h"
#include "Log.h"
#include <SysUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)

CsvWriter::CsvWriter(const AnsiString &fileSuffix):
	fp(NULL),
	failed(false),
	suffix(fileSuffix)
{
}

CsvWriter::~CsvWriter(void)
{
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
}

void CsvWriter::AddValues(const std::vector<unsigned int> &values)
{
	if (failed || values.empty())
		return;

	if (fp == NULL)
	{
		const AnsiString dir = ExtractFilePath(Application->ExeName) + "csv\\";
		ForceDirectories(dir);
		const AnsiString fileName = dir + FormatDateTime("yyyymmdd_hhmmss_zzz", Now()) + "_" + suffix + ".csv";
		fp = fopen(fileName.c_str(), "wb");
		if (fp == NULL)
		{
			LOG("Failed to open %s for writing\n", fileName.c_str());
			failed = true;
			return;
		}
		LOG("Writing values to %s\n", fileName.c_str());
	}

	const AnsiString timestamp = FormatDateTime("hh:nn:ss.zzz", Now());
	for (unsigned int i=0; i<values.size(); i++)
	{
		AnsiString text;
		text.sprintf("%s, %u\r\n", timestamp.c_str(), values[i]);
		if (fwrite(text.c_str(), text.Length(), 1, fp) != 1)
		{
			LOG("CSV write failed, closing the file\n");
			fclose(fp);
			fp = NULL;
			failed = true;
			return;
		}
	}
	fflush(fp);
}
