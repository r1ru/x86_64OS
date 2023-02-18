#include <graphics.h>

static FrameBufferInfo *frame_buffer;

void (*WritePixel)(const int x, const int y, const Pixel * const color) = NULL;

const Pixel blue  = {0xff, 0, 0};
const Pixel white = {0xff, 0xff, 0xff};

static void _writePixelRGB(const int x, const int y, const Pixel * const color) {
    const int idx = frame_buffer->pixels_per_scanline * y + x;
    uint8_t *p = (uint8_t *)frame_buffer->base + sizeof(Pixel) * idx;

    p[0] = color->red;
    p[1] = color->green;
    p[2] = color->blue;

}

static void _writePixelBGR(const int x, const int y, const Pixel * const color) {
    const int idx = frame_buffer->pixels_per_scanline * y + x;
    uint8_t *p = (uint8_t *)frame_buffer->base + sizeof(Pixel) * idx;
    
    p[0] = color->blue;
    p[1] = color->green;
    p[2] = color->red;
}

void InitializeGraphics(FrameBufferInfo *info) {
    
    // Set global variable
    frame_buffer = info;

    // Set WritePixel 
    switch(frame_buffer->pixel_format) {
        case kPixelRedGreenBlueReserved8BitPerColor:
            WritePixel = _writePixelRGB;
            break;

        case kPixelBlueGreenRedReserved8BitPerColor:
            WritePixel = _writePixelBGR;
            break;

        default:
            while(1) asm volatile ("hlt"); 
    }
}

