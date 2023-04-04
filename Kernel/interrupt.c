#include <interrupt.h>

static InterruptDescriptor IDT[256];

void setIDTEntry(InterruptVector v, uint64_t offset) {
    SegmentSelector cs = (SegmentSelector)getCS();

    IDT[v] = (InterruptDescriptor) {
        .offsetLow          = offset & 0xffffu,
        .segmentSelector    = cs,
        .attr               = (InterruptDescriptorAttribute) {
            .bits = {
                .IST    = 0,
                .Type   = 14, // Interrupt Gate
                .DPL    = 0,
                .P      = 1
            }
        },
        .offsetMiddle       = (offset >> 16) & 0xffffu,
        .offsetHigh         = offset >> 32
    };
}

void loadIDT(void) {
    _loadIDT(sizeof(IDT) - 1, (uint64_t)IDT);
}