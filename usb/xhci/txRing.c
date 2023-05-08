#include <xhci/txRing.h>

// pointer to CommandRing
static TXRing *cr; 

TXRing * newTXRing(int cap) {
    TXRing *r;
    
    if(!(r = (TXRing *)AllocMem(sizeof(TXRing), 0, 0)))
        return NULL;
    
    if(!(r->buf = (TRB *)AllocMem(sizeof(TRB) * cap, 64, 0)))
        return NULL;

    r->cap         = cap;
    r->PCS         = 1;
    r->writeIdx    = 0;

    // 最後にLinkTRBを配置
    *((LinkTRB *)&r->buf[r->cap - 1]) = (LinkTRB) {
        .RingSegmentPointerHiandLo  = (uint64_t)r->buf >> 4,
        .TRBType                    = Link,
        .TC                         = 1,
        .C                          = r->PCS
    };
    
    return r;
}

USBError initCommandRing(int cap) {
    if(!(cr = newTXRing(cap)))
        return NewErrorf(ErrMemory, "could not allocate command ring");
    
    // CommandRingの登録
    CRCRBitmap crcr = op->CRCR;
    crcr.bits.CommandRingPointer = (uint64_t)cr->buf >> 6;
    crcr.bits.RCS = cr->PCS;
    op->CRCR = crcr;

    return Nil; 
}

// CommandRing, TransferRing共用の処理
USBError PushTRB(TXRing *r, TRB *trb) {
    TRB *dest = &r->buf[r->writeIdx];

    *dest   = *trb;
    dest->C = r->PCS;
    r->writeIdx++;

    Log(
        Info,
        "[+] pushed trb@%p: type: %#x\n",
        dest,
        trb->TRBType
    );

    if (r->writeIdx == r->cap - 1) {
        r->buf[r->writeIdx].C = r->PCS;
        r->PCS = !r->PCS;
        r->writeIdx = 0;
    }

    return Nil;
}

// Doorbell Regiterに書き込みを行う
USBError RingDoorBell(int slotID, int epNumber) {
    db[slotID] = (DoorBellRegister){
        .bits = {
            .DBTarget   = slotID == 0 ? 0 : epNumber * 2 + 1,
            .DBStreamId = 0
        }
    };
    
    Log(
        Info,
        "[+] rang doorbell for slot%#x\n",
        slotID
    );

    return Nil;
}

// CommnadRingにCommand TRBを書き込んでxhcに通知する
USBError PushCommand(TRB *trb) {
    switch(trb->TRBType) {
        case NoOpCommand:
        case EnableSlotCommand:
        case AddressDeviceCommand:
            PushTRB(cr, trb);
            RingDoorBell(0, 0);
            return Nil;
        default:
            return NewErrorf(ErrCommand, "invalid command");   
    }
}

// TODO: Push2Transferを実装
