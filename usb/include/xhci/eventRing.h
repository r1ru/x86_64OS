#ifndef __EVENT_RING_H__
#define __EVENT_RING_H__

#include <stdint.h>
#include <stdalign.h>

#include <xhci/trb.h>
#include <xhci/registers.h>

#define ERSEGSIZE   0x10
typedef struct {
    TRB er_segment[ERSEGSIZE];
    int CCS; // Consumer Cycle State
    int readIdx;
} EventRing;

// EventRingSegmentTableEntry defined on p.515
typedef struct __attribute__((packed)){
    uint8_t     Rsvd                    : 6;
    uint64_t    RingSegmentBaseAddress  : 58;
    uint16_t    RingSegmentSize         : 16;
    uint16_t    Rsvd                    : 16;
    uint32_t    Rsvd                    : 32;
} EventRingSegmentTableEntry;

void initEventRing(void);
TRB * popEvent(bool *hasNext);

#endif 