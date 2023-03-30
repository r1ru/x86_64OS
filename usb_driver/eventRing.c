#include <eventRing.h>

static alignas(64)  EventRing er;
static alignas(64)  EventRingSegmentTableEntry erst[1];

void initEventRing(void) {
    er.CCS      = 1; // Event Ring`s PCS initialized to 1
    er.readIdx  = 0;
    // set event ring segment table
    erst[0].RingSegmentBaseAddress = (uint64_t)er.er_segment >> 6;
    erst[0].RingSegmentSize = ERSEGSIZE;
    // set primary interrupter
    InterrupterRegisterSet *IR0 = intr;
    ERSTSZBitmap erstsz = (ERSTSZBitmap)IR0->ERSTSZ.data;
    erstsz.bits.EventRingSegmentTableSize = 1;
    IR0->ERSTSZ.data = erstsz.data;
    ERSTBABitmap erstba = (ERSTBABitmap)IR0->ERSTBA.data;
    erstba.bits.EventRingSegmentTableBaseAddressRegister = (uint64_t)erst >> 6;
    IR0->ERSTBA.data = erstba.data;
    ERDPBitmap erdp = (ERDPBitmap)IR0->ERDP.data;
    erdp.bits.EventRingDequeuePointer = (uint64_t)er.er_segment >> 4;
    IR0->ERDP.data = erdp.data;
}

// this function is blocking
TRB * popEvent(void) {
    TRB *trb = &er.er_segment[er.readIdx++];
    InterrupterRegisterSet *IR0 = intr;

    while(trb->C != er.CCS);

    // update ERDP
    if (er.readIdx < ERSEGSIZE) {
        IR0->ERDP.data += 0x10;
        er.readIdx++;
    } else {
        IR0->ERDP.data = (uint64_t)er.er_segment;
        er.readIdx = 0;
        er.CCS = !er.CCS;
    }

    return trb;
}