#ifndef __DEVICE_PATH_TO_TEXT_PROTOCOL_H__
#define __DEVICE_PATH_TO_TEXT_PROTOCOL_H__

#include <UefiBaseType.h>
#include <Protocol/DevicePath.h>

#define EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID \
  { \
    0x8b843e20, 0x8132, 0x4852, {0x90, 0xcc, 0x55, 0x1a, 0x4e, 0x4a, 0x7f, 0x1c} \
  }

typedef 
CHAR16 *
(EFIAPI *EFI_DEVICE_PATH_TO_TEXT_PATH) (
    IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN BOOLEAN DisplayOnly,
    IN BOOLEAN AllowShortcuts
);

typedef struct {
    char _pad[8];
    EFI_DEVICE_PATH_TO_TEXT_PATH ConvertDevicePathToText;
} EFI_DEVICE_PATH_TO_TEXT_PROTOCOL;

extern EFI_GUID gEfiDevicePathToTextProtocolGuid;

#endif