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
    initSegmentation();
    
    for(int x = 0; x < info->horizontal_resolution; x++) {
        for(int y = 0; y < info->vertical_resolution; y++) {
            WritePixel(x, y, &blue);
        }
    }

    // 割り込みハンドラの登録
    printk("xhciInterruptHandler@%p\n", &xhciInterruptHandler);
    setIDTEntry(xHCIVector, (uint64_t)&xhciInterruptHandler);
    loadIDT();
    asm volatile("sti");

    int NumDevice = scanAllBus();
    printk("NumDevice: %#x\n", NumDevice);

    UsbError err = initXhc(NumDevice);

    switch (err) {
        case ErrxHCSetupCompleted:
            printk("xHC setup completed\n");
            break;
        
        default:
            printk("initXhc failed : %#x\n", err);
            while(1) asm volatile("cli\n\thlt");
    }

    pushCommand(&(TRB){.TRBType = NoOpCommand});

    while(1) {
        if(isEmpty()) {
            asm volatile("hlt");
            continue;
        }
        
        InterruptMessage msg = popMsg();

        switch(msg.ty) {
            case InterruptXHCI:
                ProcessEvent();
                break;
            default:
                continue;
        }
    }
}