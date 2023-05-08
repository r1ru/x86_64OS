#include <xhci/driver.h>

// 現在処理中のPort番号
static uint8_t addressingPortID;

USBError initXhc(int NumDevice) {
    USBError err;

    saveRegs();

    // xHCの初期化
    if(!op->USBSTS.bits.HCH)
        return NewErrorf(ErrxHC, "xHC not halted");

    USBCMDBitmap usbcmd = op->USBCMD;
    usbcmd.bits.HCRST = 1;
    op->USBCMD = usbcmd;
    while(op->USBCMD.bits.HCRST);
    while(op->USBSTS.bits.CNR);

    // port managerの初期化
    err = InitPortManager();
    if(err.code)
        return err;

    // xHCが扱えるデバイスコンテキストの数の最大値を設定する
    err = initDCBAA(NumDevice);
    if(err.code)
        return err;

    // slot managerの初期化
    err = InitSlotManager();
    if(err.code)
        return err;
    
    // Command Ringの設定
    err = initCommandRing(0x10);
    if(err.code)
        return err;

    // Event Ringの設定(Primary Interrupter)
    err = initEventRing(0x10);
    if(err.code)
        return err;
    
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

    // USB Device Managerの初期化
    err = InitDeviceManager();
    if(err.code)
        return err;

    // start xHC 
    usbcmd = op->USBCMD;
    usbcmd.bits.R_S = 1;
    op->USBCMD = usbcmd;
    while(op->USBSTS.bits.HCH);

    return Nil;
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
static USBError addressDevice(uint8_t slotID) {
    USBError err;
    // USBデバイスの情報を管理する構造体を確保
    USBDevice *dev = NewUSBDevice();
    
    // InputContextを生成
    dev->InputContext = newInputContext();
    if(!dev->InputContext)
        return NewErrorf(ErrMemory, "could not allocate inputctx");
    
    // A0(SlotContext)とA1(EP0(Default Controll Pipe))を有効化
    dev->InputContext->InputControlContext.AddContextFlags |= 0b11;
    
     // SlotContextを設定 ref: p.96
    PORTSCBitmap portsc = pr[addressingPortID - 1].PORTSC;

    dev->InputContext->SlotContext = (SlotContext) {
        .RouteString        = 0,
        .Speed              = portsc.bits.PortSpeed,
        .ContextEntries     = 1,
        .RootHubPortNumber  = addressingPortID,
    };

    // Default Control Pipe用のTransfer Ringを生成
    dev->TransferRings[0] = newTXRing(0x10);
    if(!dev->TransferRings[0])
        return NewErrorf(ErrMemory, "could not allocate transfer ring");

    // Default Controll Pipeの設定 ref: p.89
    dev->InputContext->EndpointContext[0] = (EndpointContext) {
        .EPType             = 4, // Control
        .MaxPacketSize      = portSpeed2MaxPacketSize(dev->InputContext->SlotContext.Speed),
        .MaxBurstSize       = 0,
        .TRDequeuePointer   = (uint64_t)dev->TransferRings[0]->buf >> 4,
        .DCS                = 1, // r->PCS shoud be initialied to 1
        .Interval           = 0,
        .MaxPStreams        = 0,
        .Mult               = 0,
        .CErr               = 3, 
    };

    // OutputContextを生成してdcbaaに登録
    dev->OutputContext= newDeviceContext();
    if(!dev->OutputContext)
        return NewErrorf(ErrMemory, "could not allocate outputctx");
    
    dcbaa[slotID] = dev->OutputContext;

    // Device Mangerに登録
    err = RegisterDevice(dev, slotID);
    if(err.code)
        return err;
    
    // AddressDeviceCommandを発行 ref: p.110
    return PushCommand((TRB *)&(AddressDeviceCommandTRB) {
        .InputContextPointerHiandLo = (uint64_t)dev->InputContext >> 4,
        .BSR                        = 0,
        .TRBType                    = AddressDeviceCommand,
        .SlotID                     = slotID,
    });
}

static USBError OnCompletionEvent(CommandCompletionEventTRB *trb) {
    TRB *req = (TRB *)(trb->CommandTRBPointerHiandLo << 4);
    USBError err;

    Log(
        Info,
        "[+] received trb@%p: type: %#x completion code: %#x\n",
        req,
        req->TRBType,
        trb->CompletionCode
    );

    switch(req->TRBType) {
        case EnableSlotCommand:
            // TODO: error時にCompletionCodeも通知できるようにする
            if(trb->CompletionCode != Success) {
                addressingPortID = 0;
                return NewErrorf(ErrCommand, "enable slot failed");
            }
            // Slotの状態をEnabledに遷移
            err = TransitionSlotState(trb->SlotId, SlotStateEnabled);
            if(err.code)
                return err;
            // addressDeviceを呼ぶ
            return addressDevice(trb->SlotId);
        
        case AddressDeviceCommand:
            if(trb->CompletionCode != Success) {
                addressingPortID = 0;
                return NewErrorf(ErrCommand, "address device failed");
            }
            // Slotの状態をAddressedに遷移
            err = TransitionSlotState(trb->SlotId, SlotStateAddressed);
            if(err.code)
                return err;
            return GetDeviceDescriptor(trb->SlotId);
        
        default:
            return Nil;
    }
}

static USBError OnPortStatusChangedEvent(PortStatusChangedEventTRB *trb) {
    uint8_t portID = trb->PortID;
    USBError err;

    Log(
        Info,
        "[+] port status changed: portID: %#x completion code: %#x\n",
        portID,
        trb->CompletionCode
    );

    addressingPortID = addressingPortID ? addressingPortID : portID;
    Log(
        Info,
        "[*] addressing portID: %#x\n",
        addressingPortID
    );

    PORTSCBitmap portsc = GetPORTSC(portID);

    // portの状態変化がdeviceのattachによるものだった場合
    if(portsc.bits.CSC && portsc.bits.CCS) {
        // clear CSC
        ClearPORTSCFlags(portID, _CSC);

        // USB2 protocol portで接続が検知されDisable stateになった場合
        if(!portsc.bits.PED && !portsc.bits.PR && portsc.bits.PLS == 0x7) {
            err = TransitionPortState(portID, PortStateDisabled);
            if(err.code)
                return err;
            
            if(portID == addressingPortID) {
                // リセット処理を実行
                return ResetPort(portID);
            }
            Log(
                Warning,
                "[*] delayed precessing port%#x\n",
                portID
            );
            return Nil;
        }

        // USB3 protocol portで接続が検知されEnabled stateになった場合
        if(portsc.bits.PED && !portsc.bits.PR && !portsc.bits.PLS) {
            err = TransitionPortState(portID, PortStateEnabled);
            if(err.code)
                return err;
            
            // clear PEC
            ClearPORTSCFlags(portID, _PEC);

            if(portID == addressingPortID) {
                // EnableSlotCommandを発行してスロット割り当てを実行
                return PushCommand((TRB *)&(EnableSlotCommandTRB) {
                    .TRBType    = EnableSlotCommand,
                    .SlotType   = 0,
                });
            }
            Log(
                Warning,
                "[*] delayed precessing port%#x\n",
                portID
            );
            return Nil;
        }
    }

    // USB2で発行したリセット処理が完了してEnable stateになった場合
    if(portsc.bits.PRC && portsc.bits.PED && !portsc.bits.PR && !portsc.bits.PLS) {
        err = TransitionPortState(portID, PortStateEnabled);
        if(err.code)
            return err;
        
        // clear PRC and PEC
        ClearPORTSCFlags(portID, _PRC|_PEC);

        if(portID == addressingPortID) {
            // EnableSlotCommandを発行してスロット割り当てを実行
            return PushCommand((TRB *)&(EnableSlotCommandTRB){
                .TRBType    = EnableSlotCommand,
                .SlotType   = 0,
            });
        }
        Log(
            Warning,
            "[*] delayed precessing port%#x\n",
            portID
        );
        return Nil;
    }

    // 今のところそれ以外の場合(例えばdeviceのdetach)は考慮されていない
    return NewErrorf(
        ErrEvent,
        "unexpected event factors"
    );
}

static USBError OnTransferEvent(TransferEventTRB *trb) {
    TRB *req = (TRB *)trb->TRBPointerHiandLo;

    Log(
        Info,
        "[+] received trb@%p: type: %#x completion code: %#x transfer length: %#x\n",
        req,
        req->TRBType,
        trb->CompletionCode,
        trb->TRBType,
        trb->TRBTransferLength
    );

    SetupStageTRB *setup;

    switch(req->TRBType) {
        case SetupStage:
            setup = (SetupStageTRB *)req;

            // Device Descriptorの転送だった場合
            if(setup->bRequeset == 6 && setup->wValue == 0x0100) {
                PrintDeviceDescriptor(trb->SlotID);
                GetConfiguration(trb->SlotID);
            }
            // Configuration Descriptorの転送だった場合
            if(setup->bRequeset == 6 && setup->wValue == 0x0200) {
                 PrintConfigurationDescriptor(trb->SlotID);
            }
    }

    return Nil;
}

static USBError processEvent(TRB *trb) {
    switch(trb->TRBType) {
        case CommandCompletionEvent:
            return OnCompletionEvent((CommandCompletionEventTRB *)trb);
        case PortStatsChangeEvent:
            return OnPortStatusChangedEvent((PortStatusChangedEventTRB *)trb);
        case TransferEvent:
            return OnTransferEvent((TransferEventTRB *)trb);
        default:
            return NewErrorf(
                ErrEvent,
                "unsupported event"
            );
    }
}

void ProcessEvents(void) {
    bool hasNext = false;

    // EventRingにエントリがなければreturn   
    if(!hasEvent()) return;

    do {
        PrintError(processEvent(popEvent(&hasNext)));
    } while (hasNext);
}
