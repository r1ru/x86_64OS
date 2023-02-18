#include <font.h>

const uint8_t font_bitmap[][FONT_HEIGHT] = {
    [' '] = {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000
    },
    ['!'] = {
        0b00000000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00000000,
        0b00001000,
        0b00000000
    },
    [','] = {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00001000,
        0b00010000,
        0b00000000
    },
    ['.'] = {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00001000,
        0b00000000
    },
    ['0'] = {
        0b00000000,
        0b00111100,
        0b01000110,
        0b01001010,
        0b01001010,
        0b01010010,
        0b01010010,
        0b01100010,
        0b00111100,
        0b00000000        
    },
    ['1'] = {
        0b00000000,
        0b00001000,
        0b00011000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00111100,
        0b00000000
    },
    ['2'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b00000100,
        0b00001000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b01111110,
        0b00000000
    },
    ['3'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b00000100,
        0b00011000,
        0b00011000,
        0b00000100,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['4'] = {
        0b00000000,
        0b00000100,
        0b00001100,
        0b00010100,
        0b00100100,
        0b01000100,
        0b01111110,
        0b00000100,
        0b00000100,
        0b00000000
    },
    ['5'] = {
        0b00000000,
        0b01111110,
        0b01000000,
        0b01000000,
        0b01111100,
        0b00000010,
        0b00000010,
        0b01000010,
        0b00111100,
        0b00000000      
    },
    ['6'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['7'] = {
        0b00000000,
        0b01111110,
        0b00000010,
        0b00000100,
        0b00000100,
        0b00001000,
        0b00001000,
        0b00010000,
        0b00010000,
        0b00000000
    },
    ['8'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000010,
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['9'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000010,
        0b00111110,
        0b00000010,
        0b00000010,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['A'] = {
        0b00000000,
        0b00011000,
        0b00011000,
        0b00100100,
        0b00100100,
        0b01000010,
        0b01111110,
        0b01000010,
        0b01000010,
        0b00000000
    },
    ['B'] = {
        0b00000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111100,
        0b00000000
    },
    ['C'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['D'] = {
        0b00000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111100,
        0b00000000
    },
    ['E'] = {
        0b00000000,
        0b01111110,
        0b01000000,
        0b01000000,
        0b01111100,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111110,
        0b00000000
    },
    ['F'] = {
        0b00000000,
        0b01111110,
        0b01000000,
        0b01000000,
        0b01111100,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b00000000
    },
    ['G'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000000,
        0b01000000,
        0b01001110,
        0b01000010,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['H'] = {
        0b00000000,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111110,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00000000
    },
    ['I'] = {
        0b00000000,
        0b00111100,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00111100,
        0b00000000
    },
    ['J'] = {
        0b00000000,
        0b00000010,
        0b00000010,
        0b00000010,
        0b00000010,
        0b00000010,
        0b00000010,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['K'] = {
        0b00000000,
        0b01000010,
        0b01000100,
        0b01001000,
        0b01010000,
        0b01110000,
        0b01001000,
        0b01000100,
        0b01000010,
        0b00000000
    },
    ['L'] = {
        0b00000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111100,
        0b00000000
    },
    ['M'] = {
        0b00000000,
        0b01000010,
        0b01100110,
        0b01011010,
        0b01011010,
        0b01011010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00000000
    },
    ['N'] = {
        0b00000000,
        0b01000010,
        0b01100010,
        0b01010010,
        0b01001010,
        0b01000110,
        0b01000110,
        0b01000010,
        0b01000010,
        0b00000000
    },
    ['O'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['P'] = {
        0b00000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01111100,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b00000000
    },
    ['Q'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01010010,
        0b01001010,
        0b01000110,
        0b00111110,
        0b00000000
    },
    ['R'] = {
        0b00000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01111100,
        0b01010000,
        0b01001000,
        0b01000100,
        0b01000010,
        0b00000000
    },
    ['S'] = {
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000000,
        0b00111100,
        0b00000010,
        0b00000010,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['T'] = {
        0b00000000,
        0b01111110,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00000000
    },
    ['U'] = {
        0b00000000,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
        0b00000000
    },
    ['V'] = {
        0b00000000,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00100100,
        0b00100100,
        0b00100100,
        0b00011000,
        0b00011000,
        0b00000000
    },
    ['W'] = {
        0b00000000,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01011010,
        0b01011010,
        0b00100100,
        0b00100100,
        0b00000000
    },
    ['X'] = {
        0b00000000,
        0b01000010,
        0b00100100,
        0b00100100,
        0b00011000,
        0b00011000,
        0b00100100,
        0b00100100,
        0b01000010,
        0b00000000
    },
    ['Y'] = {
        0b00000000,
        0b01000010,
        0b00100100,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00000000
    },
    ['Z'] = {
        0b00000000,
        0b01111110,
        0b00000010,
        0b00000100,
        0b00001000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b01111110,
        0b00000000
    }
};

// TODO: 入力文字が表示できるか確認する。(範囲外参照が起きないようにする)
void WriteAscii(const int x, const int y, const char c, const Pixel * const color) {
     for(int dx = 0; dx < FONT_WIDTH; dx++) {
        for(int dy = 0; dy < FONT_HEIGHT; dy++){
            if((font_bitmap[(unsigned int)c][dy] << dx) & 0b10000000)
                WritePixel(x + dx, y + dy, &white);
        }
    }
}

void WriteString(const int x, const int y, const char * const s, const Pixel * const color) {
    for(int i = 0; s[i] != '\0'; i++)
        WriteAscii(x + FONT_WIDTH * i, y, s[i], color);
}