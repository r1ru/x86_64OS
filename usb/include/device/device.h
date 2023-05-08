#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stdint.h>
#include <stddef.h>

#include <memory/allocator.h>
#include <usbError.h>
#include <xhci/context.h>
#include <xhci/txRing.h>
#include <device/descriptor.h>

// Device State defind on p.300 of USB3 Spec
typedef enum {
    DeviceStateDefault,
    DeviceStateAddress,
    DeviceStateConfigured,
} DeviceState;

// USB Deviceごとに必要な情報を保持する構造体
typedef struct {
    DeviceState                 DeviceState;
    StandardDeviceDescriptor    DeviceDescriptor;
    InputContext                *InputContext;
    DeviceContext               *OutputContext; // これはxHCが所有する。
    TXRing                      *TransferRings[16]; // Endpointは一つのデバイスにつき最大で16個
    uint8_t                     Configuration[64];
} USBDevice;

USBDevice *NewUSBDevice(void);
USBError GetDeviceDescriptor(int slotId);
USBError GetConfiguration(int slotID);

#include <device/manager.h> // 相互参照によるエラーを防ぐため

#endif