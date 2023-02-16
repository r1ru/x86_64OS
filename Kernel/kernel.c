#include "kernel.h"

void _start(UINT64 frame_buffer_base, UINT64 frame_buffer_size) {
    asm volatile(
        ".intel_syntax noprefix\n"
        "push rdx\n"
        "push rcx\n"
        "pop rdi\n"
        "pop rsi\n"
        "jmp KernelMain\n"
        ".L0: hlt\n"
        "jmp .L0\n"
    );
}

void KernelMain(UINT64 frame_buffer_base, UINT64 frame_buffer_size) {
    UINT8 *frame_buffer = (UINT8*)frame_buffer_base;

    for(UINT64 i = 0; i < frame_buffer_size; i++)
        frame_buffer[i] = i % 256;

    while(1) asm volatile("hlt");
}