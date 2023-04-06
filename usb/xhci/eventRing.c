#include <xhci/eventRing.h>

static alignas(64)  EventRing er;
static alignas(64)  EventRingSegmentTableEntry erst[1];

void initEventRing(void) {
    er.CCS      = 1; // Event Ring`s PCS initialized to 1
    er.readIdx  = 0;
    // set event ring segment table
    erst[0].RingSegmentBaseAddress = (uint64_t)er.er_segment >> 6;
    erst[0].RingSegmentSize = ERSEGSIZE;
    // set primary interrupter
    ERSTSZBitmap erstsz = intr->ERSTSZ;
    erstsz.bits.EventRingSegmentTableSize = 1;
    intr->ERSTSZ = erstsz;
    ERSTBABitmap erstba = intr->ERSTBA;
    erstba.bits.EventRingSegmentTableBaseAddressRegister = (uint64_t)erst >> 6;
    intr->ERSTBA = erstba;
    ERDPBitmap erdp = intr->ERDP;
    erdp.bits.EventRingDequeuePointer = (uint64_t)er.er_segment >> 4;
    intr->ERDP = erdp;
}

// 下位4bitはそのまま書き戻す
static void writeDequeuePointer(uint64_t p) {
    ERDPBitmap erdp = intr->ERDP;
    erdp.bits.EventRingDequeuePointer = p >> 4;
    intr->ERDP = erdp;
}

// Ensure that there is an entry in EventRing before calling popEvent
TRB * popEvent(bool *hasNext) {
    TRB *trb = &er.er_segment[er.readIdx];

    // update ERDP
    if(er.readIdx != ERSEGSIZE - 1) {
        er.readIdx++;
        writeDequeuePointer((uint64_t)&er.er_segment[er.readIdx]);
    } else {
        er.readIdx = 0;
        writeDequeuePointer((uint64_t)&er.er_segment[er.readIdx]);
        er.CCS = !er.CCS;
    }

    *hasNext = er.er_segment[er.readIdx].C == er.CCS;

    return trb;
}