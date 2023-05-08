#include <device/device.h>

// これはEnableSlotCommandが成功した後に呼ばれる。
// この時PortStateはEnabled,DeviceStateはDefaultになっている。
USBDevice *NewUSBDevice(void) {
    return (USBDevice *)AllocMem(sizeof(USBDevice), 0, 0);
}

USBError GetDescriptor(int slotID, DesctiprotTypes ty) {
    USBDevice *dev = GetDeviceBySlotID(slotID);
    TXRing *tx = dev->TransferRings[0]; // EP0用のTransferRingを使う。

    SetupStageTRB setup = (SetupStageTRB) {
        .bmRequestType      = 0b10000000,
        .bRequeset          = 6, // GET_DESCRIPTOR
        .wIndex             = 0,
        .TRBTransferLength  = 8,
        .TRT                = 3, // IN Data Stage
        .IDT                = 1,
        .IOC                = 1,
        .TRBType            = SetupStage
    };

    DataStageTRB data  = (DataStageTRB) {
        .TDSize             = 0,  
        .DIR                = 1, // IN(Read Data)
        .IOC                = 1, // コントロール転送完了時に割り込みを発生させる。
        .TRBType            = DataStage
    };

    StatusStageTRB status = (StatusStageTRB) {
        .DIR        = 0, // Out(Host-to-device)
        .IOC        = 1,
        .TRBType    = StatusStage,
    };
    
    // TODO: エラー処理
    switch(ty) {
        case DEVICE:
            setup.wValue            = 0x0100;
            setup.wLength           = sizeof(dev->DeviceDescriptor);
            data.DataBufferHiandLo  = (uint64_t)&dev->DeviceDescriptor;
            data.TRBTransferLength  = sizeof(dev->DeviceDescriptor);
            break;
        case CONFIGURATION:
            setup.wValue            = 0x0200;
            setup.wLength           = sizeof(dev->Configuration);
            data.DataBufferHiandLo  = (uint64_t)&dev->Configuration;
            data.TRBTransferLength  = sizeof(dev->Configuration);
            break;
    }

    PushTRB(tx, (TRB *)&setup);
    PushTRB(tx, (TRB *)&data);
    PushTRB(tx, (TRB *)&status);

    RingDoorBell(slotID, 0);
    return Nil; // Always Success
}

USBError GetDeviceDescriptor(int slotID) {
    return GetDescriptor(slotID, DEVICE);
}

USBError GetConfiguration(int slotID) {
    return GetDescriptor(slotID, CONFIGURATION);
}