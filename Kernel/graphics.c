#include <graphics.h>

static FrameBufferInfo *frame_buffer;

void (*WritePixel)(const int x, const int y, const Pixel *c) = NULL;

const Pixel blue = {0xff, 0, 0, 0};

static void _writePixelRGB(const int x, const int y, const Pixel *c) {
    const int idx = frame_buffer->pixels_per_scanline * y + x;
    uint8_t *p = (uint8_t *)frame_buffer->base + sizeof(Pixel) * idx;

    p[0] = c->red;
    p[1] = c->green;
    p[2] = c->blue;

}

static void _writePixelBGR(const int x, const int y, const Pixel *c) {
    const int idx = frame_buffer->pixels_per_scanline * y + x;
    uint8_t *p = (uint8_t *)frame_buffer->base + sizeof(Pixel) * idx;
    
    p[0] = c->blue;
    p[1] = c->green;
    p[2] = c->red;
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

