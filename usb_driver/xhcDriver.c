#include <xhcDriver.h>

Device xhcDev;

#define ERSEG_SIZE 0x10

// とりあえず要素は固定(TODO: malloc作る？)
alignas(64) DeviceContext* dcabaa[64];
alignas(64) CommandRing cr;
alignas(64) TRB er_segment[ERSEG_SIZE];
alignas(64) EventRingSegmentTableEntry  erst[1];

/*
BAR:
    | 31:4 | 3 | 2:1 | 0 |
    | ---- | ---- | ---- | ---- |
    | base address | prefetch enable flag | type | メモリ空間インジケータ

64bitの場合、BAR0には下位32bit、BAR1には上位32bitが入る。BAR0は下位3bitをマスクして使う */
UsbError initXhc(int NumDevice) {
    // TODO: BARを読む処理を関数に切り出す
    uint32_t lower = readData(makeAddress(xhcDev.bus, xhcDev.dev, xhcDev.func, 0x10)) & 0xfffffff0u; // mask lower 4bits
    uint64_t upper = readData(makeAddress(xhcDev.bus, xhcDev.dev, xhcDev.func, 0x14));
    uint64_t mmioBase = (upper << 32) | lower;
    printk("mmioBase: %#x\n", mmioBase);

    CapabilityRegistes *CapRegs = (CapabilityRegistes *)mmioBase;
    OperationalRegisters *OperationalRegs = (OperationalRegisters *)(mmioBase + CapRegs->CAPLENGTH);
    
    // xHCの初期化
    if(!OperationalRegs->USBSTS.HCH)
        return xHCNotHalted;

    OperationalRegs->USBCMD.HCRST = 1;
    while(OperationalRegs->USBCMD.HCRST);
    while(OperationalRegs->USBSTS.CNR);
    printk("xHC reset completed\n");

    // xHCが扱えるデバイスコンテキストの数の最大値を設定する
    printk("MaxSlots: %#x\n", CapRegs->HCSPARAMS1.MaxSlots);
    OperationalRegs->CONFIG.MaxSlotsEn = NumDevice;
    OperationalRegs->DCBAAP.DeviceContextBaseAddressArrayPointer = (uint64_t)dcabaa >> 6;

    // Command Ringの設定
    cr.PCS = 1;
    cr.writeIdx = 0;
    OperationalRegs->CRCR.CommandRingPointer = (uint64_t)&cr >> 6;
    OperationalRegs->CRCR.RCS = cr.PCS;
    printk("command ring setup completed\n");

    // Event Ringの設定(Primary Interrupter)
    erst[0].RingSegmentBaseAddress = (uint64_t)&er_segment << 6;
    erst[0].RingSegmentSize = ERSEG_SIZE;
    InterrupterRegisterSet *IR0 = (InterrupterRegisterSet *)(mmioBase + CapRegs->RTSOFF + 0x20);
    printk("IR0(Primary Interrupter Register Set)@%p\n", IR0);
    IR0->ERSTSZ.EventRingSegmentTableSize = 1;
    IR0->ERSTBA.EventRingSegmentTableBaseAddressRegister = (uint64_t)erst << 6;
    IR0->ERDP.EventRingDequeuePointer = (uint64_t)&er_segment << 4;
    printk("event ring setup completed\n");

    return xHCSetupCompleted;
}
