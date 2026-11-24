//---------------------------------------------------------------------------


#pragma hdrstop

#include "ImageExport.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\libpng\png.h"
#pragma link "png.lib"
#pragma link "zlib.lib"

//---------------------------------------------------------------------------

#pragma package(smart_init)


int WriteImage(AnsiString fileName, Graphics::TBitmap* bmp, enum ImageExportFormat format)
{
	if (format == IMAGE_EXPORT_FORMAT_BMP)
	{
		try
		{
			bmp->PixelFormat = pf8bit;
			bmp->SaveToFile(fileName);
		}
		catch (...)
		{
        	return -1;
		}
		return 0;
	}
	else if (format == IMAGE_EXPORT_FORMAT_PNG)
	{
		bmp->PixelFormat = pf24bit;
		int width = bmp->Width;
		int height = bmp->Height;

		int rc = 0;
		png_structp png_ptr = NULL;
		png_infop info_ptr = NULL;
		(void)info_ptr;
		unsigned char* line = NULL;
		(void)line;

		// Open file for writing (binary mode)
		FILE *fp = fopen(fileName.c_str(), "wb");
		if (fp == NULL) {
			rc = 1;
			goto finalize;
		}

		// Initialize write structure
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png_ptr == NULL) {
			rc = 2;
			goto finalize;
		}

		// Initialize info structure
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			rc = 3;
			goto finalize;
		}

		// Setup Exception handling
		if (setjmp(png_jmpbuf(png_ptr))) {
			rc = 4;
			goto finalize;
		}

		png_init_io(png_ptr, fp);

		// Write header (8 bit colour depth)
		png_set_IHDR(png_ptr, info_ptr, width, height,
				8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		png_write_info(png_ptr, info_ptr);

		line = (unsigned char*)malloc(width*3);
		if (line == NULL)
			goto finalize;

		// Write image data
		for (int y=0 ; y<height ; y++) {
		#if 0
			unsigned char* linePtr = (unsigned char*)bmp->ScanLine[y];
			png_write_row(png_ptr, linePtr);
		#else
			// swapping bytes for correct colors
			memcpy(line, bmp->ScanLine[y], width*3);
			for (int i=0; i<width; i++) {
				int pos1 = i*3+2;
				int pos2 = i*3;
				unsigned char tmp = line[pos1];
				line[pos1] = line[pos2];
				line[pos2] = tmp;
			}
			png_write_row(png_ptr, line);
		#endif
		}

		free(line);

		// End write
		png_write_end(png_ptr, NULL);

	finalize:
		if (fp != NULL) fclose(fp);
		if (info_ptr != NULL) {
			png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
			free(info_ptr);	// why libpng examples are skipping this?
			info_ptr = NULL;
			(void)info_ptr;
		}
		if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

		return rc;
	}
	else
	{
        return -1;
    }
}
