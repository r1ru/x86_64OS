#include <UefiLib.h>

#define MAX_STR_BUF 100
#define MEMMAP_SIZE 4096 * 4

EFI_SYSTEM_TABLE *ST;
EFI_BOOT_SERVICES *gBS;
EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *gDPTTP;
EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL *gDPFTP;
EFI_DEVICE_PATH_UTILITIES_PROTOCOL *gDPUP;

static MemoryMap map = {NULL, 0, 0, 0, 0};

// 凄い気持ち悪いけど、現状解決法がこれしかないので追加する
void  __chkstk(void){};

static EFI_STATUS OpenGOP(EFI_HANDLE image_handle, EFI_GRAPHICS_OUTPUT_PROTOCOL **gop) {
    EFI_STATUS status;
    UINTN num_gop_handles = 0;
    EFI_HANDLE* gop_handles = NULL;

    status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &num_gop_handles,
        &gop_handles);
    
    if(status)
        return status;

    status = gBS->OpenProtocol(
        gop_handles[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID**)gop,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    
    if(status)
        return status;

    gBS->FreePool(gop_handles);

    return EFI_SUCCESS;
}

VOID UefiInit(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTalble) {
    EFI_STATUS status;

    // Set global variables
    ST = SystemTalble;
    gBS = SystemTalble->BootServices;

    // Locate protocols
    status = OpenGOP(ImageHandle, &GOP);
    assert(status, L"OpenGOP");

    status = gBS->LocateProtocol(
        &gEfiDevicePathToTextProtocolGuid, 
        NULL, 
        (VOID**)&gDPTTP);
    assert(status, L"Failed to locate DPTTP");

    status = gBS->LocateProtocol(
        &gEfiDevicePathFromTextProtocolGuid,
        NULL,
        (VOID **)&gDPFTP);
    assert(status, L"Failed to locate DPFTP");

    status = ST->BootServices->LocateProtocol(
        &gEfiDevicePathUtilitiesProtocolGuid,
        NULL,
        (VOID **)&gDPUP);
    assert(status, L"Failed to locate DPUP");

    // Clear screen
    ST->ConOut->ClearScreen(ST->ConOut);

}

EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL ** root) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp;

    status = gBS->OpenProtocol(
        image_handle,
        &gEfiLoadedImageProtocolGuid,
        (VOID**)&loaded_image,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    
    if(status)
        return status;

    status = gBS->OpenProtocol(
        loaded_image->DeviceHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID**)&sfsp,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    if(status)
        return status;
    
    return sfsp->OpenVolume(sfsp, root);
}

// On success, it is caller`s responsibility to free allocated buffer
EFI_STATUS GetMemoryMap(MemoryMap *map) {
    EFI_STATUS status;
    
    status = gBS->AllocatePool(
        EfiLoaderData,
        MEMMAP_SIZE,
        &map->buffer
        );
    assert(status, L"AllocatePool");

    map -> map_size = MEMMAP_SIZE;

    status = gBS->GetMemoryMap(
        &map->map_size,
        (EFI_MEMORY_DESCRIPTOR*)map->buffer,
        &map->map_key,
        &map->descriptor_size,
        &map->descriptor_version);
    if(status) {
        gBS->FreePool(map->buffer);
        return status;
    }

    return EFI_SUCCESS;
}

EFI_STATUS ExitBootServices(EFI_HANDLE ImageHandle) {
    EFI_STATUS status;
    
    status = GetMemoryMap(&map);
    if(status) return status;
    
    status = gBS->ExitBootServices(ImageHandle, map.map_key);
    if(status) return status;

    return EFI_SUCCESS;
}

VOID putc(UINT16 c) {
    UINT16 str[2];
    str[0] = c;
    str[1] = L'\0';
    ST->ConOut->OutputString(ST->ConOut, str);
}

VOID puts(UINT16 *s) {
    ST->ConOut->OutputString(ST->ConOut, s);
}

// put hex
VOID puth(UINT64 val, UINT8 bytes) {
    UINT16 unicode_val;
    UINT16 str[MAX_STR_BUF];

    int num_digit = 2 * bytes;
    
    for(int i = num_digit - 1; 0 <= i; i--) {
        unicode_val = (UINT16)(val & 0x0f); // 16進数一桁は4bit
        if(unicode_val < 0xa)
            str[i] = unicode_val + L'0';
        else 
            str[i] = (unicode_val - 0xa) + L'A';
        val >>= 4;
    }
    str[num_digit] = L'\0';

    puts(str);
}

// 関数は成功すると0を返すように実装されている。0でなければエラーメッセージを出して無限ループする。
VOID assert(UINTN status, UINT16 *message) {
    if(status) {
        puts(message);
        puts(L":");
        puth(status, sizeof(UINTN));
        puts(L"\r\n");
        while(1) asm volatile("hlt");
    }
}

VOID Memcpy(UINT8 *dst, UINT8 *src, UINTN len) {
    while(len--)
        *dst++ = *src++;
}

VOID Bzero(UINT8 *s, UINTN n) {
    while(n--)
        *s++ = 0;
}