#ifndef __DEV_CONTEXT_H__
#define __DEV_CONTEXT_H__

#include <stdint.h>
#include <macros.h>

typedef struct __attribute__((packed)){
    uint32_t    RouteString         : 20;
    uint8_t     Speed               : 4;
    uint8_t     Rsvd                : 1;
    uint8_t     MTT                 : 1;
    uint8_t     Hub                 : 1;
    uint8_t     ContextEntries      : 5;
    uint16_t    MaxExitLatency      : 16;
    uint8_t     RootHubPortNumber   : 8;
    uint8_t     NumberOfPorts       : 8;
    uint8_t     ParentHubSlotId     : 8;
    uint8_t     ParentPortNumber    : 8;
    uint8_t     TTT                 : 2;
    uint8_t     Rsvd                : 4;
    uint16_t    InterrupterTarget   : 10;
    uint8_t     UsbDeviceAddress    : 8;
    uint32_t    Rsvd                : 19;
    uint8_t     SlotState           : 5;
} SlotContext;

typedef struct __attribute__((packed)) {
    uint8_t     EPState             : 3;
    uint8_t     Rsvd                : 5;
    uint8_t     Mult                : 2;
    uint8_t     MaxPStreams         : 5;
    uint8_t     LSA                 : 1;
    uint8_t     Interval            : 8;
    uint8_t     MaxESITPayloadHi    : 8;
    uint8_t     Rsvd                : 1;
    uint8_t     CErr                : 2;
    uint8_t     EPType              : 3;
    uint8_t     Rsvd                : 1;
    uint8_t     HID                 : 1;
    uint8_t     MaxBurstSize        : 8;
    uint16_t    MaxPacketSize       : 16;
    uint8_t     DCS                 : 1;
    uint8_t     Rsvd                : 3;
    uint64_t    TRDequeuePointer    : 60;
    uint16_t    AverageTRBLength    : 16;
    uint16_t    MaxESITPayloadLo    : 16;
} EndPointContext;

typedef struct __attribute__((packed)) {
    SlotContext     SlotContext;
    EndPointContext EPContext[31];
} DeviceContext;

#endif 