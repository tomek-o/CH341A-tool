//---------------------------------------------------------------------------

#ifndef ImageExportH
#define ImageExportH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include <Graphics.hpp>

enum ImageExportFormat {
	IMAGE_EXPORT_FORMAT_PNG = 0,
	IMAGE_EXPORT_FORMAT_BMP,
	
	IMAGE_EXPORT_FORMAT__LIMITER
};

int WriteImage(AnsiString fileName, Graphics::TBitmap* bmp, enum ImageExportFormat format);

#endif
