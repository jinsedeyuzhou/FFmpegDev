//
// Created by cxp on 2019-11-14.
//

#ifndef FFMPEGDEV_OPENGL_PIXEL_OUTPUT_H
#define FFMPEGDEV_OPENGL_PIXEL_OUTPUT_H

#include <stdint.h>

class OpenGLPixelReceiver {
public:
    virtual void ReceivePixel(uint8_t *rgba) = 0;
};

#endif //FFMPEGDEV_OPENGL_PIXEL_OUTPUT_H
