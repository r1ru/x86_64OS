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

#endif 