#include <kernel.h>

void _start(FrameBufferInfo *info) {
    asm volatile(
        ".intel_syntax noprefix\n"
        "push rcx\n"
        "pop rdi\n"
        "jmp KernelMain\n"
        ".L0: hlt\n"
        "jmp .L0\n"
    );
}

void KernelMain(FrameBufferInfo *info) {

    InitializeGraphics(info);

    for(int x = 0; x < info->horizontal_resolution; x++) {
        for(int y = 0; y < info->vertical_resolution; y++) {
            WritePixel(x, y, &blue);
        }
    }

    int i = 0;
    for(uint8_t c = '!'; c <= '~'; c++, i++) {
        WriteAscii(8 * i, 50, c, &white);
    }
    
    kprintf("%s", "HELLO WORLD!");
    kprintf("%d + %d = %d", 1, 2, 3);

    while(1) asm volatile("hlt");
}