#ifndef __SIMPLE_FILE_SYSTEM_H__
#define __SIMPLE_FILE_SYSTEM_H__

#include <UefiBaseType.h>
#include <Protocol/File.h>

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
  { \
    0x0964e5b22, 0x6459, 0x11d2, {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b} \
  }

struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef 
EFI_STATUS
(EFIAPI *EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME) (
    IN struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This,
    OUT EFI_FILE_PROTOCOL **Root
);

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL{
    UINT64 Revision;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

extern EFI_GUID gEfiSimpleFileSystemProtocolGuid;

#endif 