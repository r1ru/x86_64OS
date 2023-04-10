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

UsbError initCommandRing(int cap) {
    if(!(cr = newTXRing(cap)))
        return ErrLowMemory;
    
    printk("cr@%p cr->buf@%p\n", cr, cr->buf);

    // CommandRingの登録
    CRCRBitmap crcr = op->CRCR;
    crcr.bits.CommandRingPointer = (uint64_t)cr->buf >> 6;
    crcr.bits.RCS = cr->PCS;
    op->CRCR = crcr;

    return  ErrSuccess; 
}

// CommandRing, TransferRing共用の処理
static void pushTRB(TXRing *r, TRB *trb) {
    TRB *dest = &r->buf[r->writeIdx];

    printk(
        "trb@%p %#x\n",
        dest, 
        trb->TRBType
    );

    *dest   = *trb;
    dest->C = r->PCS;
    r->writeIdx++;

    if (r->writeIdx == r->cap - 1) {
        r->buf[r->writeIdx].C = r->PCS;
        r->PCS = !r->PCS;
        r->writeIdx = 0;
    }
}

// Doorbell Regiterに書き込みを行う
// この関数は必ず成功する
UsbError RingDoorBell(int slotID, int epNumber) {
    if(slotID == 0) {
        // slotIDが0の時はDB0に書き込み、epNumberは無視。
        db->data = 0;
    } else {
        // それ以外の場合はslotIDに対応するDBにDCI値を書き込む。
        db[slotID].data = epNumber * 2 + 1;
    }
     return ErrSuccess;
}

// CommnadRingにCommand TRBを書き込んでxhcに通知する
UsbError PushCommand(TRB *trb) {
    switch(trb->TRBType) {
        case NoOpCommand:
        case EnableSlotCommand:
        case AddressDeviceCommand:
            pushTRB(cr, trb);
            return RingDoorBell(0, 0);
        default:
            return ErrInvalidCommand;   
    }
}

// TODO: Push2Transferを実装
