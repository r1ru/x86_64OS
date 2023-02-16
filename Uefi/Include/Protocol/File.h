#ifndef __FILE_PROTOCOL_H__
#define __FILE_PROTOCOL_H__

#include <UefiBaseType.h>

struct _EFI_FILE_PROTOCOL;

#define EFI_FILE_MODE_READ 0x0000000000000001
#define EFI_FILE_MODE_WRITE 0x0000000000000002
#define EFI_FILE_MODE_CREATE 0x8000000000000000

typedef
EFI_STATUS
(EFIAPI *EFI_FILE_OPEN) (
    IN struct _EFI_FILE_PROTOCOL *This,
    OUT struct _EFI_FILE_PROTOCOL **NewHandle,
    IN CHAR16 *FileName,
    IN UINT64 OpenMode,
    IN UINT64 Attributes
);

typedef 
EFI_STATUS
(EFIAPI *EFI_FILE_CLOSE) (
    IN struct _EFI_FILE_PROTOCOL *This
);

typedef 
EFI_STATUS
(EFIAPI *EFI_FILE_READ) (
    IN struct _EFI_FILE_PROTOCOL *This,
    IN OUT UINTN *BufferSize,
    OUT VOID *Buffer
);

typedef 
EFI_STATUS
(EFIAPI *EFI_FILE_WRITE) (
    IN struct _EFI_FILE_PROTOCOL *This,
    IN OUT UINTN *BufferSize,
    IN VOID *Buffer
);

/*
@brief InformarionTypeで指定された情報をBufferに保存してサイズをBufferSizeに書き込む。
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_FILE_GET_INFO) (
    IN struct _EFI_FILE_PROTOCOL    *This,
    IN EFI_GUID                     *InformationType,
    IN OUT UINTN                    *BufferSize,
    OUT VOID                        *Buffer
);

typedef 
EFI_STATUS
(EFIAPI *EFI_FILE_FLUSH) (
    IN struct _EFI_FILE_PROTOCOL *This
);

typedef struct _EFI_FILE_PROTOCOL {
    UINT64              Revision;
    EFI_FILE_OPEN       Open;
    EFI_FILE_CLOSE      Close;
    char _pad1[8];
    EFI_FILE_READ       Read;
    EFI_FILE_WRITE      Write;
    char _pad2[16];
    EFI_FILE_GET_INFO   GetInfo;
    char _pad3[8];
    EFI_FILE_FLUSH      Flush;
}EFI_FILE_PROTOCOL;

#endif 