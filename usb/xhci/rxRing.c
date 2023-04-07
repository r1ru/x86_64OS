#include <xhci/rxRing.h>

// pointer to PrimaryEventRing
static RXRing *er;
 
EventRingSegmentTableEntry * newERST(int cap) {
    return (EventRingSegmentTableEntry *)AllocMem(sizeof(EventRingSegmentTableEntry) * cap, 64, 0);
}

RXRing * newRXRing(int cap) {
    RXRing *r;

    if(!(r = (RXRing *)AllocMem(sizeof(RXRing), 0, 0)))
        return NULL;
    
    if(!(r->segment = (TRB *)AllocMem(sizeof(TRB) * cap, 64, 0)))
        return NULL;

    r->cap          = cap;
    r->CCS          = 1;
    r->readIdx      = 0;

    return r;
}

// TODO: 
// segmentを1つ以上持てるようにする？
// PrimaryInterrupter以外を指定できるようにする 
UsbError initEventRing(int cap) {
    EventRingSegmentTableEntry *erst;

    if(!(erst = newERST(1)))
        return ErrLowMemory;
    
    if(!(er = newRXRing(cap)))
        return ErrLowMemory;
    
    erst[0].RingSegmentBaseAddress = (uint64_t)er->segment >> 6;
    erst[0].RingSegmentSize = cap;

    // set primary interrupter
    ERSTSZBitmap erstsz = intr->ERSTSZ;
    erstsz.bits.EventRingSegmentTableSize = 1;
    intr->ERSTSZ = erstsz;
    ERSTBABitmap erstba = intr->ERSTBA;
    erstba.bits.EventRingSegmentTableBaseAddressRegister = (uint64_t)erst >> 6;
    intr->ERSTBA = erstba;
    ERDPBitmap erdp = intr->ERDP;
    erdp.bits.EventRingDequeuePointer = (uint64_t)er->segment >> 4;
    intr->ERDP = erdp;

    return ErrSuccess;
}

// 下位4bitはそのまま書き戻す
static void writeDequeuePointer(uint64_t p) {
    ERDPBitmap erdp = intr->ERDP;
    erdp.bits.EventRingDequeuePointer = p >> 4;
    intr->ERDP = erdp;
}

// PrimaryEventRingにエントリがあるか調べる
// TODO: 他のEventRingでも使えるようにする
bool hasEvent(void) {
    return er->segment[er->readIdx].C == er->CCS;
}

// Ensure that there is an entry in EventRing before calling popEvent
// TODO: 他のEventRingでも使えるようにする
TRB * popEvent(bool *hasNext) {
    TRB *trb = &er->segment[er->readIdx];

    // update ERDP
    if(er->readIdx != er->cap - 1) {
        er->readIdx++;
        writeDequeuePointer((uint64_t)&er->segment[er->readIdx]);
    } else {
        er->readIdx = 0;
        writeDequeuePointer((uint64_t)&er->segment[er->readIdx]);
        er->CCS = !er->CCS;
    }

    *hasNext = hasEvent();

    return trb;
}