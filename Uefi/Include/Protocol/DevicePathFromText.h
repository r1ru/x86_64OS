#ifndef __DEVICE_PATH_FROM_TEXT_PROTOCOL_H__
#define __DEVICE_PATH_FROM_TEXT_PROTOCOL_H__

#include <UefiBaseType.h>
#include <Protocol/DevicePath.h>

#define EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL_GUID \
  { \
   0x5c99a21, 0xc70f, 0x4ad2, {0x8a, 0x5f, 0x35, 0xdf, 0x33, 0x43, 0xf5, 0x1e} \
  }

typedef 
EFI_DEVICE_PATH_PROTOCOL *
(EFIAPI *EFI_DEVICE_PATH_FROM_TEXT_NODE) (
    IN CONST CHAR16 *TextDeviceNode
);

typedef 
EFI_DEVICE_PATH_PROTOCOL *
(EFIAPI *EFI_DEVICE_PATH_FROM_TEXT_PATH) (
    IN CONST CHAR16 *TextDevicePath
);

typedef struct {
    EFI_DEVICE_PATH_FROM_TEXT_NODE ConvertTextToDeviceNode;
    EFI_DEVICE_PATH_FROM_TEXT_PATH ConvertTextToDevicePath;
} EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL;

extern EFI_GUID gEfiDevicePathFromTextProtocolGuid;

#endif