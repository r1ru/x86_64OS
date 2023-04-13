#ifndef __SLOT_MANAGER_H__
#define __SLOT_MANAGER_H__

#include <usbError.h>
#include <logger.h>
#include <memory/allocator.h>
#include <xhci/registers.h>

// Slot State defined on p.98
typedef enum {
    SlotStateDisabled,
    SlotStateEnabled,
    SlotStateDefault,
    SlotStateAddressed,
    SlotStateConfigured,
} SlotState;

typedef struct {
    int len;
    SlotState *state;
} SlotStateTable;

USBError InitSlotManager(void);
SlotState GetSlotState(int slotID);
USBError TransitionSlotState(int slotID, SlotState to);

#endif 