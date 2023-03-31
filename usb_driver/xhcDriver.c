#include <xhcDriver.h>

// とりあえず要素は固定(TODO: malloc作る？)
alignas(64) DeviceContext* dcabaa[64];

UsbError initXhc(int NumDevice) {
    saveRegs();

    // xHCの初期化
    if(!op->USBSTS.bits.HCH)
        return xHCNotHalted;

    USBCMDBitmap usbcmd = (USBCMDBitmap)op->USBCMD.data;
    usbcmd.bits.HCRST = 1;
    op->USBCMD.data = usbcmd.data;
    while(op->USBCMD.bits.HCRST);
    while(op->USBSTS.bits.CNR);
    printk("xHC reset completed\n");

    // xHCが扱えるデバイスコンテキストの数の最大値を設定する
    printk("MaxSlots: %#x\n", cap->HCSPARAMS1.bits.MaxSlots);
    CONFIGBitmap config = (CONFIGBitmap)op->CONFIG.data;
    config.bits.MaxSlotsEn = NumDevice;
    op->CONFIG.data = config.data;
    op->DCBAAP.data = (uint64_t)dcabaa;

    // Command Ringの設定
    initCommandRing();
    printk("command ring setup completed\n");

    // Event Ringの設定(Primary Interrupter)
    initEventRing();
    printk("event ring setup completed\n");

    // start xHC 
    usbcmd = (USBCMDBitmap)op->USBCMD.data;
    usbcmd.bits.R_S = 1;
    op->USBCMD.data = usbcmd.data;
    while(op->USBSTS.bits.HCH);
    printk("xHC started\n");

    // Capability Listを表示してみる
    configureMSI(xhcDev);

    // Send NoOpCommand ref p.107 (TODO: 初期化処理から分離する)
    CommandRingError err = pushCommand(NoOpCommand);
    if(err != CommandRequested) {
        printk("[error] command request failed\n");
        while(1) asm volatile("hlt");
    }
    printk("No Op Command Requeseted\n");

    // TODO: 割り込みで処理する
    CommandCompletionEventTRB *trb = (CommandCompletionEventTRB *)popEvent();
    if(trb->TRBType != CommandCompletionEvent) {
        printk("[error] unexpected event\n");
        while(1) asm volatile("hlt");
    }
    printk("received TRB\n");
   
    printk("CommandCompletionEvent\n");
    printk(
        "CommandTRBPointer@%p CompletionCode: %#x C: %#x\n",
        trb->CommandTRBPointerHiandLo << 4,
        trb->CompletionCode,
        trb->C // EventRing`s PCS flag
    );

    return xHCSetupCompleted;
}
