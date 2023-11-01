#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "TextRenderer.h"

namespace pico_ssd1306 {
	void drawText(pico_ssd1306::SSD1306 *ssd1306, const unsigned char *font, const char *text, uint8_t anchor_x,
				  uint8_t anchor_y, WriteMode mode, Rotation rotation) {
		if(!ssd1306 || !font || !text) return;

		(void)font_5x8;
		(void)font_8x8;
		(void)font_12x16;
		(void)font_16x32;

        uint8_t font_width = font[0];

        uint16_t n = 0;
        while (text[n] != '\0') {
            switch (rotation) {
                case deg0:
                    drawChar(ssd1306, font, text[n], anchor_x + (n * font_width), anchor_y, mode, rotation);
                    break;
                case deg90:
                    drawChar(ssd1306, font, text[n], anchor_x, anchor_y + (n * font_width), mode, rotation);
                    break;
            }

            n++;
        }
    }

    void drawChar(pico_ssd1306::SSD1306 *ssd1306, const unsigned char *font, char c, uint8_t anchor_x, uint8_t anchor_y,
                  WriteMode mode, Rotation rotation) {
        if(!ssd1306 || !font || c < 32) return;

        uint8_t font_width = font[0];
        uint8_t font_height = font[1];

        uint16_t seek = (c - 32) * (font_width * font_height) / 8 + 2;

        uint8_t b_seek = 0;

        for (uint8_t x = 0; x < font_width; x++) {
            for (uint8_t y = 0; y < font_height; y++) {
                if ((font[seek] >> b_seek) & 1) {
                    switch (rotation) {
						case deg0:
							ssd1306->setPixel(x + anchor_x, y + anchor_y, mode);
							break;
                        case deg90:
                            ssd1306->setPixel(-y + anchor_x + font_height, x + anchor_y, mode);
                            break;
                    }
                }
                b_seek++;
                if (b_seek == 8) {
                    b_seek = 0;
                    seek++;
                }
            }
        }
    }
}