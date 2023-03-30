#include <xhcDriver.h>

Device xhcDev;

#define ERSEG_SIZE 0x10

// とりあえず要素は固定(TODO: malloc作る？)
alignas(64) DeviceContext* dcabaa[64];
alignas(64) CommandRing cr;
alignas(64) EventRing er;
alignas(64) EventRingSegmentTableEntry  erst[1];

/*
BAR:
    | 31:4 | 3 | 2:1 | 0 |
    | ---- | ---- | ---- | ---- |
    | base address | prefetch enable flag | type | メモリ空間インジケータ

64bitの場合、BAR0には下位32bit、BAR1には上位32bitが入る。BAR0は下位4bitをマスクして使う */
UsbError initXhc(int NumDevice) {
    // TODO: BARを読む処理を関数に切り出す
    uint32_t lower = readData(makeAddress(xhcDev.bus, xhcDev.dev, xhcDev.func, 0x10)) & 0xfffffff0u; // mask lower 4bits
    uint64_t upper = readData(makeAddress(xhcDev.bus, xhcDev.dev, xhcDev.func, 0x14));
    uint64_t mmioBase = (upper << 32) | lower;
    printk("mmioBase: %#x\n", mmioBase);

    CapabilityRegistes *CapRegs = (CapabilityRegistes *)mmioBase;
    OperationalRegisters *OperationalRegs = (OperationalRegisters *)(mmioBase + CapRegs->CAPLENGTH);
    printk("CAPLENGTH: %#x OperationalRegs@ %#x\n", CapRegs->CAPLENGTH, OperationalRegs);
    
    // xHCの初期化
    if(!OperationalRegs->USBSTS.HCH)
        return xHCNotHalted;

    OperationalRegs->USBCMD.HCRST = 1;
    while(OperationalRegs->USBCMD.HCRST);
    while(OperationalRegs->USBSTS.CNR);
    printk("xHC reset completed\n");

    // xHCが扱えるデバイスコンテキストの数の最大値を設定する
    printk("MaxSlots: %#x\n", CapRegs->HCSPARAMS1.MaxSlots);
    CONFIGBitmap config = (CONFIGBitmap)OperationalRegs->CONFIG.data;
    config.bits.MaxSlotsEn = NumDevice;
    OperationalRegs->CONFIG.data = config.data;
    OperationalRegs->DCBAAP.data = (uint64_t)dcabaa;

    // Command Ringの設定
    cr.PCS = 1;
    cr.writeIdx = 0;
    CRCRBitmap crcr = (CRCRBitmap)OperationalRegs->CRCR.data;
    crcr.bits.CommandRingPointer = (uint64_t)cr.buf >> 6;
    crcr.bits.RCS = cr.PCS;
    OperationalRegs->CRCR.data = crcr.data;
    printk("command ring setup completed\n");

    // Event Ringの設定(Primary Interrupter)
    er.CCS = 1; // Event Ring`s PCS initialized to 1
    er.readIdx = 0;
    erst[0].RingSegmentBaseAddress = (uint64_t)er.er_segment >> 6;
    erst[0].RingSegmentSize = ERSEG_SIZE;
    InterrupterRegisterSet *IR0 = (InterrupterRegisterSet *)(mmioBase + CapRegs->RTSOFF + 0x20);
    printk("IR0(Primary Interrupter Register Set)@%p\n", IR0);
    ERSTSZBitmap erstsz = (ERSTSZBitmap)IR0->ERSTSZ.data;
    erstsz.bits.EventRingSegmentTableSize = 1;
    IR0->ERSTSZ.data = erstsz.data;
    ERSTBABitmap erstba = (ERSTBABitmap)IR0->ERSTBA.data;
    erstba.bits.EventRingSegmentTableBaseAddressRegister = (uint64_t)erst >> 6;
    IR0->ERSTBA.data = erstba.data;
    ERDPBitmap erdp = (ERDPBitmap)IR0->ERDP.data;
    erdp.bits.EventRingDequeuePointer = (uint64_t)er.er_segment >> 4;
    IR0->ERDP.data = erdp.data;
    printk("event ring setup completed\n");

    // start xHC 
    OperationalRegs->USBCMD.R_S = 1;
    while(OperationalRegs->USBSTS.HCH);
    printk("xHC started\n");

    // Send NoOpCommand ref p.107 (TODO: 初期化処理から分離する)
    NoOpCommandTRB *cmd = (NoOpCommandTRB *)cr.buf;
    printk("NoOpCommand@%p\n", cmd);
    kmemset(cmd, 0, sizeof(NoOpCommandTRB));
    cmd->TRBType = NoOpCommand;
    cmd->C = cr.PCS;
    cr.writeIdx++;

    DoorBellRegister *dbreg = (DoorBellRegister *)(mmioBase + CapRegs->DBOFF);
    printk("Doobell Register0@%p\n", dbreg);
    dbreg->data = 0;
    printk("No Op Command Requeseted\n");

    // TODO: 割り込みで処理する
    while(er.er_segment[0].C != er.CCS);

    printk("received TRB\n");

    printk("deqptr: %#x\n", IR0->ERDP.bits.EventRingDequeuePointer << 4);

    int n = 0;
    CommandCompletionEventTRB *p; // TODO: TRBをunion型にする？
    TRB *trb = er.er_segment;
    while(trb->C == er.CCS) {
        switch(trb->TRBType) {
            case CommandCompletionEvent:
                p = (CommandCompletionEventTRB *)trb;
                printk("CommandCompletionEvent\n");
                printk(
                    "CommandTRBPointer@%p CompletionCode: %#x C: %#x\n",
                    p->CommandTRBPointerHiandLo << 4,
                    p->CompletionCode,
                    p->C // EventRing`s PCS flag
                );
                break;
            default:
                printk("%p: %d %x %x\n", trb, er.readIdx, trb->TRBType, trb->C);
                break;
        }
    n++;
    // update deqptr
    if (er.readIdx < 0x10) {
        IR0->ERDP.data += 0x10;
        er.readIdx++;
        trb++;    
    } else {
        IR0->ERDP.data = (uint64_t)er.er_segment;
        trb = er.er_segment;
        er.readIdx = 0;
        er.CCS = !er.CCS;
    }
    }
    printk("n: %#x\n", n);
    printk("deqptr: %#x\n", IR0->ERDP.bits.EventRingDequeuePointer << 4);

    // TODO: 処理を関数に切り出す

    return xHCSetupCompleted;
}
