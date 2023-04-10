#ifndef __USB_ERROR_H__
#define __USB_ERROR_H__

typedef enum{
    // 共通のエラー
    ErrSuccess,
    ErrLowMemory,
    // xhci/driver.c用のエラー
    ErrxHCSetupCompleted,
    ErrxHCNotHalted,
    // xhci/txRing.c用のエラー
    ErrInvalidCommand,
} UsbError;

#endif 