#include <xhci/driver.h>

// とりあえず要素は固定(TODO: malloc作る？)
alignas(64) DeviceContext* dcabaa[64];

UsbError initXhc(int NumDevice) {
    saveRegs();

    // xHCの初期化
    if(!op->USBSTS.bits.HCH)
        return ErrxHCNotHalted;

    USBCMDBitmap usbcmd = op->USBCMD;
    usbcmd.bits.HCRST = 1;
    op->USBCMD = usbcmd;
    while(op->USBCMD.bits.HCRST);
    while(op->USBSTS.bits.CNR);
    printk("xHC reset completed\n");

    // xHCが扱えるデバイスコンテキストの数の最大値を設定する
    printk("MaxSlots: %#x\n", cap->HCSPARAMS1.bits.MaxSlots);
    CONFIGBitmap config = op->CONFIG;
    config.bits.MaxSlotsEn = NumDevice;
    op->CONFIG = config;
    op->DCBAAP = (DCBAAPBitmap) {
        .bits.DeviceContextBaseAddressArrayPointer = (uint64_t)dcabaa >> 6
    };

    // Command Ringの設定
    initCommandRing();
    printk("command ring setup completed\n");

    // Event Ringの設定(Primary Interrupter)
    initEventRing();
    printk("event ring setup completed\n");

    // 割り込みの設定(Primary Interrupter)
    IMODBitmap imod = intr->IMOD;
    imod.bits.IMODI = 4000;
    intr->IMOD = imod;
    IMANBitmap iman = intr->IMAN;
    iman.bits.IP = 1;
    iman.bits.IE = 1;
    intr->IMAN = iman;
    usbcmd = op->USBCMD;
    usbcmd.bits.INTE = 1;
    op->USBCMD = usbcmd;
    MessageAddressBitmap    msgAddr = {
        .bits = {
            .DM = 0,
            .RH = 0,
            .DestinationId = (uint8_t)(*((uint32_t *)0xfee00020) >> 24),
            .OFEEH = 0xfeeu
        }
    };
    MessageDataBitmap       msgData = {
        .bits = {
            .TM = 1,
            .LV = 1,
            .DM = 0,
            .Vector = xHCIVector
        }
    };
    configureMSI(xhcDev, msgAddr, msgData);

    // start xHC 
    usbcmd = op->USBCMD;
    usbcmd.bits.R_S = 1;
    op->USBCMD = usbcmd;
    while(op->USBSTS.bits.HCH);
    printk("xHC started\n");

    return ErrxHCSetupCompleted;
}

static void OnCompletionEvent(CommandCompletionEventTRB *trb) {
    printk(
        "CommandTRBPointer@%p CompletionCode: %#x C: %#x\n",
        trb->CommandTRBPointerHiandLo << 4,
        trb->CompletionCode,
        trb->C // EventRing`s PCS flag
    );
}

static void OnPortStatusChangedEvent(PortStatusChangedEventTRB *trb) {
    uint8_t i = trb->PortID;
    // PortIDは1始まり
    PORTSCBitmap portsc = pr[i - 1].PORTSC;

    printk(
        "PortID: %#x ComplettionCode: %#x\n",
        i,
        trb->CompletionCode
    );
    // TODO: fix here
    if (portsc.bits.CSC != 1 || portsc.bits.CCS != 1) {
        printk("[error] something went wrong\n");
        return;
    }

    // Clear CSC
    portsc.bits.CSC = 1;
    portsc.data &= FLAGMASK;
    pr[i - 1].PORTSC = portsc;

    printk("port%#x enabled\n", i);
}

void ProcessEvent(void) {
    TRB *trb;
    bool hasNext;

    do {
        trb = popEvent(&hasNext);

        switch (trb->TRBType) {
            case CommandCompletionEvent:
                OnCompletionEvent((CommandCompletionEventTRB *)trb);
                break;
            case PortStatsChangeEvent:
                OnPortStatusChangedEvent((PortStatusChangedEventTRB *)trb);
                break;
            default:
                printk(
                    "TRBType: %#x C: %#x\n",
                    trb->TRBType,
                    trb->C
                );
                break;
        }
    } while (hasNext);
}
