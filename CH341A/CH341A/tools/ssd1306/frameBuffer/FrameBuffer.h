#ifndef SSD1306_FRAMEBUFFER_H
#define SSD1306_FRAMEBUFFER_H

#include <string.h>

/// \brief Set frame buffer to 1024 bytes, witch is 128*64 / 8
///
/// For 128x32 displays it's still 1024 due to how memory mapping works on ssd1306.
/// This is explained in readme.md
#define FRAMEBUFFER_SIZE 1024

/// \brief Framebuffer class contains a pointer to buffer and functions for interacting with it
class FrameBuffer {
    unsigned char * buffer;
public:
    /// Constructs frame buffer and allocates memory for buffer
    FrameBuffer();

    /// Destroys frame buffer and frees buffer memory
    ~FrameBuffer();

    /// \brief Performs OR logical operation on selected and provided byte
    ///
    /// ex. if byte in buffer at position n is 0b10001111 and provided byte is 0b11110000 the buffer at position n becomes 0b11111111
    /// \param n - byte offset in buffer array to work on
    /// \param byte - provided byte to make operation
    void byteOR(int n, unsigned char byte);

    /// \brief Performs AND logical operation on selected and provided byte
    ///
    /// ex. if byte in buffer at position n is 0b10001111 and provided byte is 0b11110000 the buffer at position n becomes 0b10000000
    /// \param n - byte offset in buffer array to work on
    /// \param byte - provided byte to make operation
    void byteAND(int n, unsigned char byte);

    /// \brief Performs XOR logical operation on selected and provided byte
    ///
    /// ex. if byte in buffer at position n is 0b10001111 and provided byte is 0b11110000 the buffer at position n becomes 0b0111111
    /// \param n - byte offset in buffer array to work on
    /// \param byte - provided byte to make operation
    void byteXOR(int n, unsigned char byte);

    /// Replaces pointer with one pointing to a different buffer
    void setBuffer(unsigned char * new_buffer);

    /// Zeroes out the buffer aka set buffer to all 0
    void clear();

    /// Returns a pointer to the buffer
    unsigned char * get();
};


#endif //SSD1306_FRAMEBUFFER_H
