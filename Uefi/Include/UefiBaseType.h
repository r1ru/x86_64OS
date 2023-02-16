#ifndef __UEFI_BASE_TYPE_H__
#define __UEFI_BASE_TYPE_H__

#include <stdint.h>
#include <stdbool.h>

// Common EFI Data Types (defined in p.20)
typedef uint64_t    UINTN;
typedef uint8_t     UINT8;
typedef int16_t     INT16;
typedef uint16_t    UINT16;
typedef int32_t     INT32;
typedef uint32_t    UINT32;
typedef uint64_t    UINT64;
typedef uint16_t    CHAR16;
typedef void        VOID;
typedef bool        BOOLEAN;
typedef UINTN       EFI_STATUS;
typedef VOID*       EFI_HANDLE;
typedef VOID*       EFI_EVENT;
typedef UINTN       EFI_TPL;
typedef UINT64      EFI_PHYSICAL_ADDRESS;
typedef UINT64      EFI_VIRTUAL_ADDRESS;

// Maximum values for common EFI Data Types
#define MAX_UINT64  ((UINT64)0xFFFFFFFFFFFFFFFFULL)

typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8 Data4[8];
} EFI_GUID;

#define EFIAPI 
#define IN 
#define OUT 
#define OPTIONAL
#define CONST const 

// status codes 
#define EFI_SUCCESS     0
#define EFI_ERROR	    0x8000000000000000
#define EFI_UNSPPORTED  (EFI_ERROR | 3)
#define EFI_BUFFER_TOO_SMALL (EFI_ERROR | 5)

#endif 