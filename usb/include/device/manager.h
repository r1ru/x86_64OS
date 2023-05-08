#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__

#include <stdint.h>

#include <usbError.h>
#include <xhci/registers.h>
#include <memory/allocator.h>
#include <device/device.h>
#include <slot/manager.h>

typedef struct {
    int len;
    USBDevice **device;
} USBDeviceTable;

USBError InitDeviceManager(void);
USBError RegisterDevice(USBDevice *dev, int slotID);
USBDevice * GetDeviceBySlotID(int slotId);

#endif