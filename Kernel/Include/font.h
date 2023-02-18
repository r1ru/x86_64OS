#ifndef __FONT_H__
#define __FONT_H__

#include <graphics.h>

#define FONT_WIDTH 8
#define FONT_HEIGHT 10

extern const uint8_t font_bitmap[][FONT_HEIGHT];
void WriteAscii(const int x, const int y, const unsigned char c, const Pixel * const color);

#endif 