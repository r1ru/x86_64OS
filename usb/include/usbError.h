#ifndef __USB_ERROR_H__
#define __USB_ERROR_H__

typedef enum{
    // xhci/driver用のエラー
    ErrxHCSetupCompleted,
    ErrxHCNotHalted,
    ErrLowMemory,
    // xhci/commandRing用のエラー
    ErrCommandRequested,
    ErrInvalidCommand,
} UsbError;

#endif 