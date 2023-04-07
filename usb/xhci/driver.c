#include <xhci/driver.h>

// とりあえず要素は固定(TODO: malloc作る？)
alignas(64) DeviceContext* dcabaa[64];

// TODO: fix this?
static uint8_t addressingPortID;

UsbError initXhc(int NumDevice) {
    UsbError err;

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
    if((err = initCommandRing(0x10)))
        return err;
    
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

/*
// TODO: PSIVとの対応関係を標準的なものであることを仮定せず、xHCI Extended Capabilitiesを読む
static uint16_t portSpeed2MaxPacketSize(uint8_t speed) {
    switch(speed) {
        case 3:
            return 64;
        case 4:
            return 512;
        default:
            return 8;
    }
}

// TODO: 関数に切り出す
static UsbError addressDevice(uint8_t slotID) {
    InputContext *p = (InputContext *)AllocMem(sizeof(InputContext));
    // A0(SlotContext)とA1(EP0(Default Controll Pipe))を有効化
    p->InputControlContext.AddContextFlags |= 0b11;
    // SlotContextを設定
    p->SlotContext = (SlotContext) {
        .RootHubPortNumber  = addressingPortID,
        .Speed              = pr[addressingPortID - 1].PORTSC.bits.PortSpeed,
    };
    // Default Controll Pipeの設定
    p->EndpointContext[0] = (EndpointContext) {
        .EPType         = 4,
        .MaxPacketSize  = portSpeed2MaxPacketSize(p->SlotContext.Speed),
        .MaxBurstSize   = 0,
        .DCS            = 1,
        .Interval       = 0,
        .MaxPStreams    = 0,
        .Mult           = 0,
        .CErr           = 3, 
    };
    // AddressDeviceCommandを発行
    // TODO: エラーをerror.hにまとめる
    return pushCommand((TRB *)&(AddressDeviceCommandTRB) {
        .InputContextPointerHiandLo = (uint64_t)p >> 4,
        .SlotID                     = slotID,
    });
}

static UsbError enableSlot(uint8_t id) {
    DeviceContext *p = (DeviceContext *)AllocMem(sizeof(DeviceContext));

    if(!p)
        return ErrLowMemory;
    
    dcabaa[id] = p;


} */

static void OnCompletionEvent(CommandCompletionEventTRB *trb) {
    TRB *req = (TRB *)(trb->CommandTRBPointerHiandLo << 4);

    printk(
        "res@%p: CompletionCode: %#x ",
        req,
        trb->CompletionCode
    );

    switch(req->TRBType) {
        case EnableSlotCommand:
            // TODO: CompletionCodeがsuccessで無かった場合を考慮
            printk(
                "SlotID: %#x",
                trb->SlotId
            );

    }

    printk("\n");

}

/*
static void OnPortStatusChangedEvent(PortStatusChangedEventTRB *trb) {
    uint8_t i = trb->PortID;
    // PortIDは1始まり
    PORTSCBitmap portsc = pr[i - 1].PORTSC;

    printk(
        "PortID: %#x CompletionCode: %#x\n",
        i,
        trb->CompletionCode
    );
    // TODO: fix here
    if (portsc.bits.CSC != 1 || portsc.bits.CCS != 1) {
        printk("[error] something went wrong\n");
        return;
    }

    // Clear CSC
    portsc.data &= FLAGMASK;
    portsc.bits.CSC = 1;
    pr[i - 1].PORTSC = portsc;

    printk("port%#x enabled\n", i);
    addressingPortID = i;

    // Enable Slot
    pushCommand(&(TRB){.TRBType = EnableSlotCommand});
} */

void ProcessEvent(void) {
    TRB *trb;
    bool hasNext = false;

    // EventRingにエントリがなければreturn   
    if(!hasEvent()) return;

    do {
        trb = popEvent(&hasNext);

        switch (trb->TRBType) {
            case CommandCompletionEvent:
                OnCompletionEvent((CommandCompletionEventTRB *)trb);
                break;
            /*
            case PortStatsChangeEvent:
                OnPortStatusChangedEvent((PortStatusChangedEventTRB *)trb);
                break; */
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
