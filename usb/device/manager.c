#include <device/manager.h>

// Slotに関連付けられている全てのデバイスを管理するテーブル
static USBDeviceTable table;

USBError InitDeviceManager(void) {
    table = (USBDeviceTable) {
        .len    = op->CONFIG.bits.MaxSlotsEn,
        .device = (USBDevice **)AllocMem(sizeof(USBDevice) * op->CONFIG.bits.MaxSlotsEn,
            0,
            0
        )
    };

    if(!table.device)
        return NewErrorf(ErrMemory, "could not allocate device table");
    
    return Nil;
}

USBError RegisterDevice(USBDevice *dev, int slotID) {
    if(table.device[slotID - 1]->DeviceState != DeviceStateDefault)
        return NewErrorf(ErrDevice, "slot%#x in use", slotID);
    
    if(dev->DeviceState != DeviceStateDefault)
        return NewErrorf(ErrDevice, "invalid device state:%#x", dev->DeviceState);
    
    dev->DeviceState = DeviceStateAddress;
    table.device[slotID - 1] = dev;

    return Nil;
}

USBDevice * GetDeviceBySlotID(int slotId) {
    return table.device[slotId - 1];
}
