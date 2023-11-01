#ifndef SSD1306_SSD1306_H
#define SSD1306_SSD1306_H

#include <string.h>
#include "frameBuffer/FrameBuffer.h"
#include <stdint.h>

class CH341A;

namespace pico_ssd1306 {
    /// Register addresses from datasheet
    enum REG_ADDRESSES {
        SSD1306_CONTRAST = 0x81,
        SSD1306_DISPLAYALL_ON_RESUME = 0xA4,
        SSD1306_DISPLAYALL_ON = 0xA5,
        SSD1306_INVERTED_OFF = 0xA6,
        SSD1306_INVERTED_ON = 0xA7,
        SSD1306_DISPLAY_OFF = 0xAE,
        SSD1306_DISPLAY_ON = 0xAF,
        SSD1306_DISPLAYOFFSET = 0xD3,
        SSD1306_COMPINS = 0xDA,
        SSD1306_VCOMDETECT = 0xDB,
        SSD1306_DISPLAYCLOCKDIV = 0xD5,
        SSD1306_PRECHARGE = 0xD9,
        SSD1306_MULTIPLEX = 0xA8,
        SSD1306_LOWCOLUMN = 0x00,
        SSD1306_HIGHCOLUMN = 0x10,
        SSD1306_STARTLINE = 0x40,
        SSD1306_MEMORYMODE = 0x20,
        SSD1306_MEMORYMODE_HORZONTAL = 0x00,
        SSD1306_MEMORYMODE_VERTICAL = 0x01,
        SSD1306_MEMORYMODE_PAGE = 0x10,
        SSD1306_COLUMNADDR = 0x21,
        SSD1306_PAGEADDR = 0x22,
        SSD1306_COM_REMAP_OFF = 0xC0,
        SSD1306_COM_REMAP_ON = 0xC8,
        SSD1306_CLUMN_REMAP_OFF = 0xA0,
        SSD1306_CLUMN_REMAP_ON = 0xA1,
        SSD1306_CHARGEPUMP = 0x8D,
        SSD1306_EXTERNALVCC = 0x1,
        SSD1306_SWITCHCAPVCC = 0x2,
    };

    /// \enum pico_ssd1306::Size
	enum Size {
		/// Display size W128xH64
		W128xH64,
		/// Display size W128xH32
		W128xH32
	};

	/// \enum pico_ssd1306::WriteMode
    enum WriteMode {
        /// sets pixel on regardless of its state
        ADD = 0,
        /// sets pixel off regardless of its state
        SUBTRACT = 1,
        /// inverts pixel, so 1->0 or 0->1
        INVERT = 2,
    };

    /// \class SSD1306 ssd1306.h "pico-ssd1306/ssd1306.h"
    /// \brief SSD1306 class represents i2c connection to display
    class SSD1306 {
    private:
		CH341A &ch341a;
		uint8_t address;
		Size size;

		FrameBuffer frameBuffer;

		uint8_t width, height;

		bool inverted;

		/// \brief Sends single 8bit command to ssd1306 controller
		/// \param command - byte to be sent to controller
		void cmd(unsigned char command);

	public:
		/// \brief SSD1306 constructor initialized display and sets all required registers for operation
		/// \param i2CInst - i2c instance. Either i2c0 or i2c1
		/// \param Address - display i2c address. usually for 128x32 0x3C and for 128x64 0x3D
		/// \param size - display size. Acceptable values W128xH32 or W128xH64
        SSD1306(CH341A &ch341a, uint8_t Address, Size size);

        /// \brief Set pixel operates frame buffer
        /// x is the x position of pixel you want to change. values 0 - 127
        /// y is the y position of pixel you want to change. values 0 - 31 or 0 - 63
        /// \param x - position of pixel you want to change. values 0 - 127
        /// \param y - position of pixel you want to change. values 0 - 31 or 0 - 63
        /// \param mode - mode describes setting behavior. See WriteMode doc for more information
        void setPixel(int16_t x, int16_t y, WriteMode mode = ADD);

        /// \brief Sends frame buffer to display so that it updated
        void sendBuffer(void);

        /// \brief Adds bitmap image to frame buffer
        /// \param anchorX - sets start point of where to put the image on the screen
        /// \param anchorY - sets start point of where to put the image on the screen
        /// \param image_width - width of the image in pixels
        /// \param image_height - height of the image in pixels
        /// \param image - pointer to uint8_t (unsigned char) array containing image data
        /// \param mode - mode describes setting behavior. See WriteMode doc for more information
        void addBitmapImage(int16_t anchorX, int16_t anchorY, uint8_t image_width, uint8_t image_height, uint8_t *image,
                            WriteMode mode = ADD);

        /// \brief Manually set frame buffer. make sure it's correct size of 1024 bytes
        /// \param buffer - pointer to a new buffer
        void setBuffer(unsigned char *buffer);

        /// \brief Flips the display
        /// \param orientation - 0 for not flipped, 1 for flipped display
        void setOrientation(bool orientation);


        /// \brief Clears frame buffer aka set all bytes to 0
        void clear();

        /// \brief Inverts screen on hardware level. Way more efficient than setting buffer to all ones and then using WriteMode subtract.
        void invertDisplay();

        /// \brief Sets display contrast according to ssd1306 documentation
        /// \param contrast - accepted values of 0 to 255 to set the contrast
        void setContrast(unsigned char contrast);

        /// \brief Turns display off
        void turnOff();

        /// \brief Turns display on
		void turnOn();
    };

}

#endif //SSD1306_SSD1306_H