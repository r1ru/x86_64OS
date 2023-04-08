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

UsbError pushCommand(TRB *trb) {
    printk("req@%p ", &cr->buf[cr->writeIdx]);
    
    switch(trb->TRBType) {
        case NoOpCommand:
            *(NoOpCommandTRB *)&cr->buf[cr->writeIdx++] = (NoOpCommandTRB) {
                .TRBType    = NoOpCommand,
                .C          = cr->PCS
            };
            printk("NoOpCommand\n");
            break;
        case EnableSlotCommand:
            *(EnableSlotCommandTRB *)&cr->buf[cr->writeIdx++] = (EnableSlotCommandTRB) {
                .TRBType    = EnableSlotCommand,
                .SlotType   = 0,
                .C          = cr->PCS
            };
            printk("EnableSlotCommand\n");
            break;
        case AddressDeviceCommand:
            *(AddressDeviceCommandTRB *)&cr->buf[cr->writeIdx++] = (AddressDeviceCommandTRB) {
                .TRBType                    = AddressDeviceCommand,
                .InputContextPointerHiandLo = ((AddressDeviceCommandTRB*)trb)->InputContextPointerHiandLo,
                .SlotID                     = ((AddressDeviceCommandTRB*)trb)->SlotID,
                .C                          = cr->PCS,
            };
            printk("AddressDeviceCommand\n");
            break;
        default:
            return ErrInvalidCommand;
    }

    if (cr->writeIdx == cr->cap - 1) {
        ((LinkTRB *)&cr->buf[cr->writeIdx])->C = cr->PCS;
        cr->PCS = !cr->PCS;
        cr->writeIdx = 0;
    }

    // Ring Doorbell
    db->data = 0;
    return ErrCommandRequested;
}