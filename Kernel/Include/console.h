#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <graphics.h>
#include <font.h>
#include <kstring.h>

typedef struct {
    int         x;          // absolute coordinates of conole
    int         y;
    int         rows;
    int         columns;
    int         cursor_x;   // relative coordinates of cursor
    int         cursor_y;
    const Pixel *fg_color;
    const Pixel *bg_color;
    char        *buf;
} Console;

void putString(const char *s);

#endif 