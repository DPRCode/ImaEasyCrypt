//
// Created by janek on 14.06.23.
//

#ifndef IMAEASYCRYPT_PIXEL_H
#define IMAEASYCRYPT_PIXEL_H


#include <cstdint>

class Pixel {
public:
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t alpha;

    Pixel() = default;
    Pixel(int r, int g, int b);
};


#endif //IMAEASYCRYPT_PIXEL_H
