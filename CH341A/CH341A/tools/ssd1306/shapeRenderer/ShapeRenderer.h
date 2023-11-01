#ifndef SSD1306_SHAPERENDERER_H
#define SSD1306_SHAPERENDERER_H

#include <math.h>
#include "../ssd1306.h"

namespace pico_ssd1306{

    /// \brief Draws a line from x0, y0 to x1, y1.
    /// It supports all drawing angles
    /// \param ssd1306 - is the pointer to a SSD1306 object aka an initialised display
    /// \param x0, y0, x1, y1 are the start and end coordinates between which the line will be drawn
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    void drawLine (pico_ssd1306::SSD1306 *ssd1306, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, pico_ssd1306::WriteMode mode = pico_ssd1306::ADD);

    /// \brief Draws a 1px wide rectangle between x0, y0 and x1, y1
    /// \param x_start, x_end, y_start, y_end - corner points for the rectangle
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    void drawRect (pico_ssd1306::SSD1306 *ssd1306 , uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, pico_ssd1306::WriteMode mode = pico_ssd1306::ADD);

    /// \brief Fills a rectangle from x0, y0 to x1, y1
    /// \param x_start, x_end, y_start, y_end - corner points for the rectangle
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    void fillRect (pico_ssd1306::SSD1306 *ssd1306 , uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, pico_ssd1306::WriteMode mode = pico_ssd1306::ADD);
}

#endif //SSD1306_SHAPERENDERER_H
