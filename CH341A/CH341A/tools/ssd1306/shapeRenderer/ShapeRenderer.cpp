#include "ShapeRenderer.h"

void pico_ssd1306::drawLine(pico_ssd1306::SSD1306 *ssd1306, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
                            pico_ssd1306::WriteMode mode) {
    int x, y, dx, dy, dx0, dy0, px, py, xe, ye, i;
    dx = x1 - x0;
    dy = y1 - y0;
    dx0 = fabs(dx);
    dy0 = fabs(dy);
    px = 2 * dy0 - dx0;
    py = 2 * dx0 - dy0;
    if (dy0 <= dx0) {
        if (dx >= 0) {
            x = x0;
            y = y0;
            xe = x1;
        } else {
            x = x1;
            y = y1;
            xe = x0;
        }
        ssd1306->setPixel(x, y, mode);
        for (i = 0; x < xe; i++) {
            x = x + 1;
            if (px < 0) {
                px = px + 2 * dy0;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    y = y + 1;
                } else {
                    y = y - 1;
                }
                px = px + 2 * (dy0 - dx0);
            }
            ssd1306->setPixel(x, y, mode);
        }
    } else {
        if (dy >= 0) {
            x = x0;
            y = y0;
            ye = y1;
        } else {
            x = x1;
            y = y1;
            ye = y0;
        }
        ssd1306->setPixel(x, y, mode);
        for (i = 0; y < ye; i++) {
            y = y + 1;
            if (py <= 0) {
                py = py + 2 * dx0;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    x = x + 1;
                } else {
                    x = x - 1;
                }
                py = py + 2 * (dx0 - dy0);
            }
            ssd1306->setPixel(x, y, mode);
        }
    }
}

void pico_ssd1306::drawRect(pico_ssd1306::SSD1306 *ssd1306, uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end,
                            pico_ssd1306::WriteMode mode) {
    drawLine(ssd1306, x_start, y_start, x_end, y_start, mode);
    drawLine(ssd1306, x_start, y_end, x_end, y_end, mode);
    drawLine(ssd1306, x_start, y_start, x_start, y_end, mode);
    drawLine(ssd1306, x_end, y_start, x_end, y_end, mode);
}

void pico_ssd1306::fillRect(pico_ssd1306::SSD1306 *ssd1306, uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end,
                            pico_ssd1306::WriteMode mode) {
    for (uint8_t x = x_start; x <= x_end; x++) {
        for (uint8_t y = y_start; y <= y_end; y++) {
            ssd1306->setPixel(x, y, mode);
        }
    }
}
