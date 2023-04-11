#ifndef __USB_ERROR_H__
#define __USB_ERROR_H__

typedef enum{
    // 共通のエラー
    ErrSuccess,
    ErrLowMemory,
    ErrUnexpectedEventFactors,
    // xhci/driver.c用のエラー
    ErrxHCSetupCompleted,
    ErrxHCNotHalted,
    ErrEnableSlotFailed,
    ErrAddressDeviceFailed,
    ErrUnsupportedEvent,
    ErrProcessingDelayed,
    // xhci/txRing.c用のエラー
    ErrInvalidCommand,
    // port/manager.c用のエラー
    ErrInvalidPort,
    ErrInvalidPortStateTransition,

} UsbError;

#endif 