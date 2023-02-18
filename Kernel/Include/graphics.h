#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <stdint.h>
#include <Protocol/GraphicsOutput.h>

#define NULL    (void*)0

// PixelFormat supported by kernel
typedef enum {
    kPixelRedGreenBlueReserved8BitPerColor, 
    kPixelBlueGreenRedReserved8BitPerColor,
} PixelFormat;

typedef struct {
    uint64_t    base;
    uint32_t    horizontal_resolution;
    uint32_t    vertical_resolution;   
    uint32_t    pixels_per_scanline;
    PixelFormat pixel_format; 
} FrameBufferInfo;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
} Pixel;

extern const Pixel blue;
extern void (*WritePixel)(const int x, const int y, const Pixel *c);

void InitializeGraphics(FrameBufferInfo *info);

#endif