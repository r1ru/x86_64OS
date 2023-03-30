#ifndef __RING_H__
#define __RING_H__

#include <stdint.h>
#include <trb.h>

typedef struct {
    TRB er_segment[0x10];
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

#endif 