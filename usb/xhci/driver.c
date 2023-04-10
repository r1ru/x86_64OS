#include <xhci/driver.h>

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
    if((err = initDCBAA(NumDevice)))
        return err;

    // Command Ringの設定
    if((err = initCommandRing(0x10)))
        return err;
    printk("command ring setup completed\n");

    // Event Ringの設定(Primary Interrupter)
    if((err = initEventRing(0x10)))
        return err;
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

// TODO: 関数に切り出す、addressingPortID = 0;を直す
static UsbError addressDevice(uint8_t slotID) {
    
    // InputContextの生成と初期化
    InputContext *inputctx = newInputContext();
    if(!inputctx) {
        addressingPortID = 0;
        return ErrLowMemory;
    }
    printk("inputctx@%p\n", inputctx);

    // A0(SlotContext)とA1(EP0(Default Controll Pipe))を有効化
    inputctx->InputControlContext.AddContextFlags |= 0b11;

    printk("addressingPortID: %#x\n", addressingPortID);

     // SlotContextを設定 ref: p.96
    PORTSCBitmap portsc = pr[addressingPortID - 1].PORTSC;

    inputctx->SlotContext = (SlotContext) {
        .RouteString        = 0,
        .Speed              = portsc.bits.PortSpeed,
        .ContextEntries     = 1,
        .RootHubPortNumber  = addressingPortID,
    };

    // Default Control Pipe用のTransfer Ringを生成
    TXRing *r = newTXRing(0x10);
    if(!r) {
        addressingPortID = 0;
        return ErrLowMemory;
    }
    printk("transfer ring@%p buf@%p\n", r, r->buf);

    // Default Controll Pipeの設定 ref: p.89
    inputctx->EndpointContext[0] = (EndpointContext) {
        .EPType             = 4, // Control
        .MaxPacketSize      = portSpeed2MaxPacketSize(inputctx->SlotContext.Speed),
        .MaxBurstSize       = 0,
        .TRDequeuePointer   = (uint64_t)r->buf >> 4,
        .DCS                = 1, // r->PCS shoud be initialied to 1
        .Interval           = 0,
        .MaxPStreams        = 0,
        .Mult               = 0,
        .CErr               = 3, 
    };

    // Output Contextを生成してdcbaaに登録
    DeviceContext *outputctx = newDeviceContext();
    if(!outputctx) {
        addressingPortID = 0;
        return ErrLowMemory;
    }
    printk("outputctx@%p\n", outputctx);
    
    dcbaa[slotID] = outputctx;

    // AddressDeviceCommandを発行 ref: p.110
    return pushCommand((TRB *)&(AddressDeviceCommandTRB) {
        .TRBType                    = AddressDeviceCommand,
        .SlotID                     = slotID,
        .InputContextPointerHiandLo = (uint64_t)inputctx >> 4,
    });
}

static void OnCompletionEvent(CommandCompletionEventTRB *trb) {
    TRB *req = (TRB *)(trb->CommandTRBPointerHiandLo << 4);

    printk(
        "res@%p: CompletionCode: %#x ",
        req,
        trb->CompletionCode
    );

    switch(req->TRBType) {
        case EnableSlotCommand:
            printk(
                "SlotID: %#x\n",
                trb->SlotId
            );
            if(trb->CompletionCode != Success) {
                printk("[error] enable slot failed\n");
                addressingPortID = 0;
                break;
            }
            // addressDeviceを呼ぶ
            printk("addressDevice : %#x\n", addressDevice(trb->SlotId));
            break;
        // TODO: addressingPortID = 0;を直す
        case AddressDeviceCommand:
            printk("AddressDeviceCommand\n");
            addressingPortID = 0;
            break;
        
        default:
            printk("\n");
    }
}

static void OnPortStatusChangedEvent(PortStatusChangedEventTRB *trb) {
    uint8_t portID = trb->PortID;
    
    printk(
        "PortID: %#x CompletionCode: %#x\n",
        portID,
        trb->CompletionCode
    );

    // 現在処理中のPortなら処理を継続。処理中のポートがなければこのポートの処理を開始する
    // TODO: ここ直す
    if(addressingPortID) {
        if(portID != addressingPortID)
            return;
    } else {
        addressingPortID = portID;
    }

    PortRegisterSet *prs = &pr[portID - 1];
    PORTSCBitmap portsc = prs->PORTSC;

    // portの状態変化がdeviceのattachによるものだった場合
    if(portsc.bits.CSC && portsc.bits.CCS) {
        // clear CSC
        portsc.data     &= FLAGMASK;
        portsc.bits.CSC = 1;
        prs->PORTSC     = portsc;

        // USB3 protocol portで接続が検知されEnabled stateになった場合
        if(portsc.bits.PED && !portsc.bits.PR && !portsc.bits.PLS) {
            // clear PEC
            portsc.data     &= FLAGMASK;
            portsc.bits.PEC = 1;
            prs->PORTSC     = portsc;
            // EnableSlotCommandを発行してスロット割り当てを実行
            printk("enabling slot for port%#x\n", addressingPortID);
            pushCommand(&(TRB){.TRBType = EnableSlotCommand});
            return;
        }

        // USB2 protocol portで接続が検知されDisable stateになった場合
        if(!portsc.bits.PED && !portsc.bits.PR && portsc.bits.PLS == 0x7) {
            // リセット処理を実行
            printk("resetting port%#x\n", addressingPortID);
            portsc.data &= FLAGMASK;
            portsc.bits.PR = 1;
            prs->PORTSC = portsc;
            return;
        }
    }

    // USB2で発行したリセット処理が完了してEnable stateになった場合
    if(portsc.bits.PRC && portsc.bits.PED && !portsc.bits.PR && !portsc.bits.PLS) {
        // clear PRC
        portsc.data     &= FLAGMASK;
        portsc.bits.PRC = 1;
        prs->PORTSC     = portsc;
        // EnableSlotCommandを発行してスロット割り当てを実行
        printk("enabling slot for port%#x\n", addressingPortID);
        pushCommand(&(TRB){.TRBType = EnableSlotCommand});
        return;
    }

    // 今のところそれ以外の場合(例えばdeviceのdetach)は考慮されていない
    printk("[error] unexpected event factors\n");
    return;
}

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
