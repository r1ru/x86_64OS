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

    int NumDevice = scanAllBus();
    printk("NumDevice: %#x\n", NumDevice);

    UsbError err = initXhc(NumDevice);

    switch (err) {
        case xHCSetupCompleted:
            printk("xHC setup completed\n");
            goto hlt;
        case xHCNotHalted:
            printk("[error] xHC not halted\n");
            goto hlt;
    }
    
    hlt:
    while(1) asm volatile("hlt");
}