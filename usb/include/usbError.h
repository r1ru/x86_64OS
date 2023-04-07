#ifndef __USB_ERROR_H__
#define __USB_ERROR_H__

typedef enum{
    // 共通のエラー
    ErrSuccess,
    ErrLowMemory,
    // xhci/driver用のエラー
    ErrxHCSetupCompleted,
    ErrxHCNotHalted,
    // xhci/commandRing用のエラー
    ErrCommandRequested,
    ErrInvalidCommand,
} UsbError;

#endif 