#include <console.h>

#define ROWS 25
#define COLUMNS 80
/* 
    現状メモリ管理を実装していないのでmallocが使えないため、仕方なくglobal変数にしている。
*/
static char buffer[ROWS][COLUMNS + 1];
static Console stdout = {
    .x = 100,
    .y = 100,
    .rows = ROWS, 
    .columns = COLUMNS, 
    .cursor_x = 0, 
    .cursor_y = 0, 
    .fg_color = &white, 
    .bg_color = &black,
    .buf = (char*)buffer
};

static void clearScreen(void) {
    for(int i = stdout.x; i < stdout.x + COLUMNS * FONT_WIDTH; i++) {
        for(int j = stdout.y; j < stdout.y + ROWS * FONT_HEIGHT; j++) {
            WritePixel(i, j , stdout.bg_color);
        }
    }
}

static void scroll(void) {
    for(int row = 0; row < ROWS - 1; row ++) {
        kmemcpy(buffer[row], buffer[row + 1], COLUMNS + 1); // null byteまでcopy
        WriteString(stdout.x, stdout.y + row * FONT_HEIGHT, buffer[row], stdout.fg_color);
    }
    kmemset(buffer[ROWS - 1], 0, COLUMNS + 1);
}

static void newLine(void) {
    stdout.cursor_x = 0;
    if(stdout.cursor_y < ROWS - 1)
        stdout.cursor_y++;
    else {
        clearScreen();
        scroll();        
    }
}

void putString(const char *s) {
    while(*s) {
        if(*s == '\n')
            newLine(); // cursor_yをいじるのはnewLineのみ。
        else if(stdout.cursor_x < COLUMNS - 1) {
            WriteAscii(
                stdout.x + stdout.cursor_x * FONT_WIDTH, 
                stdout.y + stdout.cursor_y * FONT_HEIGHT,
                *s, 
                stdout.fg_color
            );
            buffer[stdout.cursor_y][stdout.cursor_x] = *s;
            stdout.cursor_x++;
        }
        s++;
    }
}

