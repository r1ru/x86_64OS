#include <xhcDriver.h>

Device xhcDev;

/*
BAR:
    | 31:4 | 3 | 2:1 | 0 |
    | ---- | ---- | ---- | ---- |
    | base address | prefetch enable flag | type | メモリ空間インジケータ

64bitの場合、BAR0には下位32bit、BAR1には上位32bitが入る。BAR1は下位3bitをマスクして使う */
int initXhc(void) {
    uint64_t lower = readData(makeAddress(xhcDev.bus, xhcDev.dev, xhcDev.func, 0x10)) & 0xfffffff00Lu; // mask lower 3bits
    uint32_t upper = readData(makeAddress(xhcDev.bus, xhcDev.dev, xhcDev.func, 0x14));
    uint64_t mmioBase = (upper << 32) | lower;
    printk("mmioBase: %#x\n", mmioBase);

    CapabilityRegistes *CapRegs = (CapabilityRegistes *)mmioBase;
    OperationalRegisters *OperationalRegs = (OperationalRegisters *)(mmioBase + CapRegs->CAPLENGTH);
    
    if(!OperationalRegs->USBSTS.HCH) {
        printk("[Error] xHC Not Halted");
        while(1) __asm__ volatile("hlt");
    }

    OperationalRegs->USBCMD.HCRST = 1;
    while(OperationalRegs->USBCMD.HCRST);
    while(OperationalRegs->USBSTS.CNR);

    printk("xHC Reset Completed");
    
    return 0;
}
