#ifndef __DEVICE_PATH_UTILITIES_PROTOCOL_H__
#define __DEVICE_PATH_UTILITIES_PROTOCOL_H__

#include <UefiBaseType.h>
#include <Protocol/DevicePath.h>

#define EFI_DEVICE_PATH_UTILITIES_PROTOCOL_GUID \
  { \
    0x379be4e, 0xd706, 0x437d, {0xb0, 0x37, 0xed, 0xb8, 0x2f, 0xb7, 0x72, 0xa4} \
  }

typedef 
EFI_DEVICE_PATH_PROTOCOL *
(EFIAPI *EFI_DEVICE_PATH_UTILS_APPEND_NODE) (
    IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN CONST EFI_DEVICE_PATH_PROTOCOL *DeviceNode
);

typedef struct {
    char _pad[24];
    EFI_DEVICE_PATH_UTILS_APPEND_NODE AppendDeviceNode;
} EFI_DEVICE_PATH_UTILITIES_PROTOCOL;

EFI_GUID gEfiDevicePathUtilitiesProtocolGuid;

#endif