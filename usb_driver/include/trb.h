#ifndef __TRB_H__
#define __TRB_H__

#include <macros.h>
#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint64_t    Parameter   : 64;
    uint32_t    Status      : 32;
    uint8_t     C           : 1;
    uint16_t    Rsvd        : 9;
    uint8_t     TRBType     : 6;
    uint16_t    Control     : 16; 
} TRB;

// Command TRBs defined on p.487
typedef struct __attribute__((packed)){
    uint32_t    Rsvd[3];
    uint8_t     C       : 1;
    uint16_t    Rsvd    : 9;
    uint8_t     TRBType : 6;
    uint16_t    Rsvd    : 16;
} NoOpCommandTRB;

// Event TRBs defined on p.477
typedef struct __attribute__((packed)) {
    uint8_t     Rsvd                        : 4;
    uint64_t    CommandTRBPointerHiandLo    : 60;
    uint32_t    CommandCompletionParameter  : 24;
    uint8_t     CompletionCode              : 8;
    uint8_t     C                           : 1;
    uint16_t    Rsvd                        : 9;
    uint8_t     TRBType                     : 6;
    uint8_t     VFId                        : 8;
    uint8_t     SlotId                      : 8;
} CommandCompletionEventTRB;

#endif 