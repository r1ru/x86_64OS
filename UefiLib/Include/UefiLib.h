#ifndef __UEFI_LIB_H__
#define __UEFI_LIB_H__

#include <UefiBaseType.h>
#include <UefiSpec.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePathFromText.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/DevicePathUtilities.h>
#include <Protocol/File.h>
#include <Protocol/SimpleFileSystem.h>
#include <FileInfo.h>

#define NULL (void*)0

typedef struct{
    VOID                    *buffer;
    UINTN                   map_size;
    UINTN                   map_key;
    UINTN                   descriptor_size;
    UINT32                  descriptor_version;
} MemoryMap;

extern EFI_SYSTEM_TABLE *ST;
extern EFI_BOOT_SERVICES *gBS;
extern EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
extern EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *gDPTTP;
extern EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL *gDPFTP;
extern EFI_DEVICE_PATH_UTILITIES_PROTOCOL *gDPUP;

VOID UefiInit(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTalble);
EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL ** root);
EFI_STATUS GetMemoryMap(MemoryMap *map);
EFI_STATUS ExitBootServices(EFI_HANDLE ImageHandle);

VOID putc(UINT16 c);
VOID puts(UINT16 *s);
VOID puth(UINT64 val, UINT8 bytes);
VOID assert(UINTN status, UINT16 *message);
VOID Memcpy(UINT8* dst, UINT8* src, UINTN len);
VOID Bzero(UINT8 *s, UINTN n);

#endif 