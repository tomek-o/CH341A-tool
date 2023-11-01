#ifndef SSD1306_TEXTRENDERER_H
#define SSD1306_TEXTRENDERER_H

#include "../ssd1306.h"

#include "5x8_font.h"
#include "8x8_font.h"
#include "12x16_font.h"
#include "16x32_font.h"

namespace pico_ssd1306{

    /// \enum pico_ssd1306::Rotation
    enum Rotation{
        /// deg0 - means no rotation
        deg0,
        /// deg 90 - means 90 deg rotation
        deg90,
    };

    /// \brief Draws a single glyph on the screen
    /// \param ssd1306 - pointer to a SSD1306 object aka initialised display
    /// \param font - pointer to a font data array
    /// \param c - char to be drawn
    /// \param anchor_x, anchor_y - coordinates setting where to put the glyph
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    /// \param rotation - either rotates the char by 90 deg or leaves it unrotated
    void drawChar(pico_ssd1306::SSD1306 *ssd1306, const unsigned char * font, char c, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode = ADD, Rotation rotation = deg0);

    /// \brief Draws text on screen
    /// \param ssd1306 - pointer to a SSD1306 object aka initialised display
    /// \param font - pointer to a font data array
    /// \param text - text to be drawn
    /// \param anchor_x, anchor_y - coordinates setting where to put the text
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    /// \param rotation - either rotates the text by 90 deg or leaves it unrotated
    void drawText(pico_ssd1306::SSD1306 *ssd1306, const unsigned char * font, const char * text, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode = ADD, Rotation rotation = deg0);
}

#endif //SSD1306_TEXTRENDERER_H
