#ifndef __RX_RING_H__
#define __RX_RING_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <usbError.h>
#include <memory/allocator.h>
#include <xhci/trb.h>
#include <xhci/registers.h>

typedef struct __attribute__((packed)) {
    TRB *segment;
    int cap;    // RingBufferのEntry数
    int CCS;    // Consumer Cycle State
    int readIdx;
} RXRing;

// EventRingSegmentTableEntry defined on p.515
typedef struct __attribute__((packed)){
    uint8_t     Rsvd                    : 6;
    uint64_t    RingSegmentBaseAddress  : 58;
    uint16_t    RingSegmentSize         : 16;
    uint16_t    Rsvd                    : 16;
    uint32_t    Rsvd                    : 32;
} EventRingSegmentTableEntry;

EventRingSegmentTableEntry * newERST(int cap);
RXRing * newRXRing(int cap);
UsbError initEventRing(int cap);
bool hasEvent(void);
TRB * popEvent(bool *hasNext);

#endif 