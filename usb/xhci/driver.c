#include <xhci/driver.h>

// 現在処理中のPort番号
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

    // port managerの初期化
    if((err = InitPortManager()))
        return err;

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

// TODO: 関数に切り出す
// メモリ確保に失敗したときはaddressingPortID = 0;にするべきだが現状Freeがないので回復することはない。
static UsbError addressDevice(uint8_t slotID) {
    
    // InputContextの生成と初期化
    InputContext *inputctx = newInputContext();
    if(!inputctx)
        return ErrLowMemory;
    printk("inputctx@%p\n", inputctx);

    // A0(SlotContext)とA1(EP0(Default Controll Pipe))を有効化
    inputctx->InputControlContext.AddContextFlags |= 0b11;

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
    if(!r)
        return ErrLowMemory;
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
    if(!outputctx)
        return ErrLowMemory;
    printk("outputctx@%p\n", outputctx);
    
    dcbaa[slotID] = outputctx;

    // AddressDeviceCommandを発行 ref: p.110
    return PushCommand((TRB *)&(AddressDeviceCommandTRB) {
        .TRBType                    = AddressDeviceCommand,
        .SlotID                     = slotID,
        .InputContextPointerHiandLo = (uint64_t)inputctx >> 4,
    });
}

static UsbError OnCompletionEvent(CommandCompletionEventTRB *trb) {
    TRB *req = (TRB *)(trb->CommandTRBPointerHiandLo << 4);

    printk(
        "res@%p: Type: %#x CompletionCode: %#x\n",
        req,
        req->TRBType,
        trb->CompletionCode
    );

    switch(req->TRBType) {
        case EnableSlotCommand:
            // TODO: error時にCompletionCodeも通知できるようにする
            if(trb->CompletionCode != Success) {
                addressingPortID = 0;
                return ErrEnableSlotFailed;
            }
            // addressDeviceを呼ぶ
            return addressDevice(trb->SlotId);
        
        case AddressDeviceCommand:
            if(trb->CompletionCode != Success) {
                addressingPortID = 0;
                return ErrAddressDeviceFailed;
            }
            // TODO: ここに処理を追加
            return ErrSuccess;
        
        default:
            return ErrSuccess;
    }
}

static UsbError OnPortStatusChangedEvent(PortStatusChangedEventTRB *trb) {
    uint8_t portID = trb->PortID;
    UsbError err;

    printk(
        "PortID: %#x CompletionCode: %#x\n",
        portID,
        trb->CompletionCode
    );

    addressingPortID = addressingPortID ? addressingPortID : portID;

    PORTSCBitmap portsc = GetPORTSC(portID);

    // portの状態変化がdeviceのattachによるものだった場合
    if(portsc.bits.CSC && portsc.bits.CCS) {
        // clear CSC
        ClearPORTSCFlags(portID, _CSC);

        // USB2 protocol portで接続が検知されDisable stateになった場合
        if(!portsc.bits.PED && !portsc.bits.PR && portsc.bits.PLS == 0x7) {
            
            if((err = TransitionState(portID, PortStateDisabled)))
                return err;
            
            if(portID == addressingPortID) {
                // リセット処理を実行
                printk("resetting port%#x\n", addressingPortID);
                return ResetPort(portID);
            }
            return ErrProcessingDelayed;
        }

        // USB3 protocol portで接続が検知されEnabled stateになった場合
        if(portsc.bits.PED && !portsc.bits.PR && !portsc.bits.PLS) {
            if((err = TransitionState(portID, PortStateEnabled)))
                return err;
            
            // clear PEC
            ClearPORTSCFlags(portID, _PEC);

            if(portID == addressingPortID) {
                // EnableSlotCommandを発行してスロット割り当てを実行
                printk("enabling slot for port%#x\n", addressingPortID);
                return PushCommand((TRB *)&(EnableSlotCommandTRB) {
                    .TRBType    = EnableSlotCommand,
                    .SlotType   = 0,
                });
            }
            return ErrProcessingDelayed;
        }
    }

    // USB2で発行したリセット処理が完了してEnable stateになった場合
    if(portsc.bits.PRC && portsc.bits.PED && !portsc.bits.PR && !portsc.bits.PLS) {
        
        if((err = TransitionState(portID, PortStateEnabled)))
            return err;
        
        // clear PRC and PEC
        ClearPORTSCFlags(portID, _PRC|_PEC);

        if(portID == addressingPortID) {
            // EnableSlotCommandを発行してスロット割り当てを実行
            printk("enabling slot for port%#x\n", addressingPortID);
            return PushCommand((TRB *)&(EnableSlotCommandTRB){
                .TRBType    = EnableSlotCommand,
                .SlotType   = 0,
            });
        }
         return ErrProcessingDelayed;
    }

    // 今のところそれ以外の場合(例えばdeviceのdetach)は考慮されていない
    return ErrUnexpectedEventFactors;
}

static UsbError processEvent(TRB *trb) {
    switch(trb->TRBType) {
        case CommandCompletionEvent:
            return OnCompletionEvent((CommandCompletionEventTRB *)trb);
        case PortStatsChangeEvent:
            return OnPortStatusChangedEvent((PortStatusChangedEventTRB *)trb);
        default:
            return ErrUnsupportedEvent;
    }
}

void ProcessEvents(void) {
    bool hasNext = false;

    // EventRingにエントリがなければreturn   
    if(!hasEvent()) return;

    do {
        printk("processEvent: %#x\n", processEvent(popEvent(&hasNext)));
    } while (hasNext);
}
