#ifndef __TX_RING_H__
#define __TX_RING_H__

#include <stdint.h>
#include <stddef.h>

#include <usbError.h>
#include <memory/allocator.h>
#include <xhci/trb.h>
#include <xhci/registers.h>

typedef struct __attribute__((packed)){
    TRB *buf;   // 64byte alignされている必要がある。
    int cap;    // RingBufferのEntry数
    int PCS;    // Producer Cycle State
    int writeIdx;
} TXRing;

TXRing * newTXRing(int cap);
USBError initCommandRing(int cap);
USBError RingDoorBell(int slotID, int epNumber);
USBError PushCommand(TRB *trb);

#endif 