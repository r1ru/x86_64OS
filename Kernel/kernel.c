#include <kernel.h>

void _start(FrameBufferInfo *info) {
    asm volatile(
        ".intel_syntax noprefix\n"
        "push rcx\n"
        "pop rdi\n"
        "call KernelMain\n"
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
    setIDTEntry(xHCIVector, (uint64_t)&xhciInterruptHandler);
    loadIDT();
    asm volatile("sti");

    int NumDevice = scanAllBus();

    USBError err = initXhc(NumDevice);
    if(err.code){
        PrintError(err);
        while(1) asm volatile("cli\n\thlt");
    }
    
    PushCommand(&(TRB){.TRBType = NoOpCommand});

    while(1) {
        if(isEmpty()) {
            asm volatile("hlt");
            continue;
        }
        
        InterruptMessage msg = popMsg();

        switch(msg.ty) {
            case InterruptXHCI:
                ProcessEvents();
                break;
            default:
                continue;
        }
    }
}