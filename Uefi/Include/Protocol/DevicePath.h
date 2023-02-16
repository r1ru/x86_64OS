#ifndef __DEVICE_PATH_PROTOCOL_H__
#define __DEVICE_PATH_PROTOCOL_H__

#include <UefiBaseType.h>

#define EFI_DEVICE_PATH_PROTOCOL_GUID \
  { \
    0x09576e91, 0x6d3f, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b} \
  }

typedef struct {
    UINT8 Type;
    UINT8 SubType;
    UINT8 Length[2];
} EFI_DEVICE_PATH_PROTOCOL;

extern EFI_GUID gEfiDevicePathProtocolGuid;

#endif