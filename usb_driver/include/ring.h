#ifndef __RING_H__
#define __RING_H__

#include <stdint.h>

// TODO: ここ直す
typedef uint16_t TRB;

typedef struct {
    TRB buf[0x10];
    int PCS; // Producer Cycle State
    int writeIdx;
} CommandRing;

// defined on p.515
typedef struct __attribute__((packed)){
    uint8_t     Rsvd                    : 6;
    uint64_t    RingSegmentBaseAddress  : 58;
    uint16_t    RingSegmentSize         : 16;
    uint16_t    Rsvd                    : 16;
} EventRingSegmentTableEntry;

#endif 