#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() {
    this->buffer = new unsigned char[FRAMEBUFFER_SIZE];
}

FrameBuffer::~FrameBuffer() {
    delete[] this->buffer;
}

void FrameBuffer::byteOR(int n, unsigned char byte) {
    // return if index outside 0 - buffer length - 1
    if (n > (FRAMEBUFFER_SIZE-1)) return;
    this->buffer[n] |= byte;
}

void FrameBuffer::byteAND(int n, unsigned char byte) {
    // return if index outside 0 - buffer length - 1
    if (n > (FRAMEBUFFER_SIZE-1)) return;
    this->buffer[n] &= byte;
}

void FrameBuffer::byteXOR(int n, unsigned char byte) {
    // return if index outside 0 - buffer length - 1
    if (n > (FRAMEBUFFER_SIZE-1)) return;
    this->buffer[n] ^= byte;
}


void FrameBuffer::setBuffer(unsigned char *new_buffer) {
    // free buffer memory to prevent memory leak
    delete[] this->buffer;
    this->buffer = new_buffer;
}

void FrameBuffer::clear() {
    //zeroes out the buffer via memset function from string library
    memset(this->buffer, 0, FRAMEBUFFER_SIZE);
}

unsigned char *FrameBuffer::get() {
    return this->buffer;
}
