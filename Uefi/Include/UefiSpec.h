#ifndef __UEFI_SPEC_H__
#define __UEFI_SPEC_H__

#include <UefiBaseType.h>
#include <UefiMultiPhase.h>
#include <Protocol/SimpleTextInput.h>
#include <Protocol/SimpleTextOutput.h>
#include <Protocol/DevicePath.h>

// EFI_BOOT_SERVICES
typedef struct {
    UINT32                  Type;
    EFI_PHYSICAL_ADDRESS    PhysicalStart;
    EFI_VIRTUAL_ADDRESS     VirtualStart;
    UINT64                  NumberOfPages;
    UINT64                  Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef enum {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} EFI_ALLOCATE_TYPE;

typedef 
EFI_STATUS
(EFIAPI *EFI_ALLOCATE_PAGES) (
    IN EFI_ALLOCATE_TYPE        Type,
    IN EFI_MEMORY_TYPE          MemoryType,
    IN UINTN                    Pages,
    IN OUT EFI_PHYSICAL_ADDRESS *Memory
);

typedef
EFI_STATUS
(EFIAPI *EFI_FREE_PAGES) (
    IN EFI_PHYSICAL_ADDRESS Memory,
    IN UINTN                Pages
);

/*
@param MemoryMapSize    MemoryMapを格納するbufferのサイズ。書き込まれたサイズが返る。
@param MemoryMap        MemoryMapを書き込むアドレス
@param MapKey           MemoryMapを識別するための値を格納する場所を指定。
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_GET_MEMORY_MAP) (
    IN OUT UINTN                *MemoryMapSize,
    OUT EFI_MEMORY_DESCRIPTOR   *MemoryMap,
    OUT UINTN                   *MapKey,
    OUT UINTN                   *DescriptorSize,
    OUT UINT32                  *DescriptorVersion
);

// AllocatePagesとの違いはどこから確保するかを指定できない事。
typedef 
EFI_STATUS
(EFIAPI *EFI_ALLOCATE_POOL) (
    IN EFI_MEMORY_TYPE PoolType,
    IN UINTN Size,
    OUT VOID **Buffer
);

typedef 
EFI_STATUS
(EFIAPI *EFI_FREE_POOL) (
    IN VOID *Buffer
);

// Event Types
#define EVT_TIMER                           0x80000000
#define EVT_RUNTIME                         0x40000000
#define EVT_NOTIFY_WAIT                     0x00000100
#define EVT_NOTIFY_SIGNAL                   0x00000200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES       0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE   0x60000202

typedef 
VOID 
(EFIAPI *EFI_EVENT_NOTIFY) (
    IN EFI_EVENT Event,
    IN VOID *Context
);

/*
@param Type             上記のイベントタイプのどれかを指定
@param NotifyTpl        イベント通知関数のタスク優先度レベル
@param NotifyFunction   イベント発生時に実行する関数
@param NotifyContext    通知関数へ渡す引数を設定
@param Event            生成されたイベントを格納するポインタ
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_CREATE_EVENT) (
    IN UINT32 Type,
    IN EFI_TPL NotifyTpl,
    IN EFI_EVENT_NOTIFY NOtifyFunction, OPTIONAL
    IN VOID *NotifyContext, OPTIONAL
    OUT EFI_EVENT *Event
);

typedef enum {
    TimerCancel,    //設定されているトリガー時間をキャンセル
    TimerPeriodic,  //現時刻からの周期的なトリガー時間を設定
    TimerRelative   //現時刻からの1回のみのトリガー時間を設定
} EFI_TIMER_DELAY;

typedef 
EFI_STATUS
(EFIAPI *EFI_SET_TIMER) (
    IN EFI_EVENT Event,
    IN EFI_TIMER_DELAY Type,
    IN UINT64 TriggerTime
);

typedef 
EFI_STATUS
(EFIAPI *EFI_WAIT_FOR_EVENT) (
    IN UINTN NumberOfEvents,
    IN EFI_EVENT *Event,
    OUT UINTN *Index
);

typedef 
EFI_STATUS
(EFIAPI *EFI_LOCATE_PROTOCOL) (
    IN EFI_GUID *Protocol,
    IN VOID *Registration OPTIONAL,
    OUT VOID **Interface
);

/* 
@param BootPolicy ブートマネージャから呼ばれたことを表す。SourceBufferがNULLなら無視される。
@param ParentImageHandle この関数を呼んだimageのhadnle
@param DevicePath ロードするimageのDevicePath
@param SourceBuffer NULLでなければロードされるイメージのコピーがあるメモリアドレス
*/
typedef  
EFI_STATUS
(EFIAPI *EFI_IMAGE_LOAD) (
    IN BOOLEAN BootPolicy,
    IN EFI_HANDLE ParentImageHandle,
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
    IN VOID *SourceBuffer OPTIONAL,
    IN UINTN SourceSize,
    OUT EFI_HANDLE *ImageHandle
);

/*
@param ImageHandle 実行するimageのhandle
@param ExitDataSize ExitDataのサイズへのポインタ。ExitDataがNULLの場合はULLを指定する
@param ExitData BootServices->Exit()で終了した場合に呼び出し元へ返されるデータへのポインタ
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_IMAGE_START) (
    IN EFI_HANDLE ImageHandle,
    OUT UINTN *ExitDataSize,
    OUT CHAR16 **ExitData OPTIONAL
);

typedef 
EFI_STATUS
(EFIAPI *EFI_EXIT) (
    IN EFI_HANDLE   ImageHandle,
    IN EFI_STATUS   ExitStatus,
    IN UINTN        ExitDataSize,
    IN CHAR16       *ExitData OPTIONAL
);

/*
@param Mapkey その時点で最新のメモリマップのkey
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_EXIT_BOOT_SERVICES) (
    IN EFI_HANDLE   ImageHandle,
    IN UINTN        MapKey
);

typedef 
EFI_STATUS
(EFIAPI *EFI_SET_WATCHDOG_TIMER) (
    IN UINTN Timeout, // これを0にすると無効化できる。その他の引数は0かNULLでいい。
    IN UINT64 WatchdogCode,
    IN UINTN DataSize,
    IN CHAR16 *WatchdogData OPTIONAL
);

typedef enum {
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

typedef 
EFI_STATUS 
(EFIAPI *EFI_LOCATE_HANDLE_BUFFER) (
    IN EFI_LOCATE_SEARCH_TYPE SearchType,
    IN EFI_GUID *Protocol OPTIONAL,
    IN VOID *SearchKey OPTIONAL,
    OUT UINTN *NumHandles,
    OUT EFI_HANDLE **Buffer
);

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL    0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL           0x00000002

/* 
@param Handle オープンするプロトコルで扱う対象のハンドルを指定
@param Protocol プロトコルのGUID
@param Interface プロトコル構造体のポインタを格納するための変数
@param AgentHandle  OpenProtocolを呼んでいるイメージのハンドル。つまり自分自身のイメージハンドル
@param ConrtollerHandle OpenProtocolを呼んでいるのがUEFI driverである場合に指定する。それ以外はNULL 

*/
typedef 
EFI_STATUS 
(EFIAPI *EFI_OPEN_PROTOCOL) (
    IN EFI_HANDLE Handle,
    IN EFI_GUID *Protocol,
    OUT VOID **Interface OPTIONAL,
    IN EFI_HANDLE AgentHandle,
    IN EFI_HANDLE ControllerHandle, 
    IN UINT32 Attributes 
);

typedef struct {
    char _pad1[40];

    // Memory Services
    EFI_ALLOCATE_PAGES  AllocatePages;
    EFI_FREE_PAGES      FreePages;
    EFI_GET_MEMORY_MAP  GetMemoryMap;
    EFI_ALLOCATE_POOL   AllocatePool;
    EFI_FREE_POOL       FreePool;

    // Event & Timer Services
    EFI_CREATE_EVENT CreateEvent;
    EFI_SET_TIMER SetTimer;
    EFI_WAIT_FOR_EVENT WaitForEvent;
    char _pad4[24];

    // Protocol Handler Services
    char _pad5[72];

    // Image Services
    EFI_IMAGE_LOAD          LoadImage;
    EFI_IMAGE_START         StartImage;
    EFI_EXIT                Exit;
    char _pad7[8];
    EFI_EXIT_BOOT_SERVICES  ExitBootServices;

    // Miscellaeous Services 
    char _pad8[16];
    EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;

    // DriverSupport Services
    char _pad9[16];

    // Open and Close Protocol Services
    EFI_OPEN_PROTOCOL OpenProtocol;
    char _pad10[16];

    // Library Services
    char _pad11[8];
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL LocateProtocol;

} EFI_BOOT_SERVICES;

// EFI_RUNTIME_SERVICES

// EFI_RESET_TYPE
typedef enum {
    EFiResetCold,               // 再起動
    EfiResetWarm,               // CPUのみをリセットする。これがサポートされていないときはColdを実行する必要がある
    EfiResetShutdown,           // シャットダウン
    EfiResetPlatformSpecific    // ?
} EFI_RESET_TYPE;

/*
@param ResetType    上記のResetTypeの内どれかを指定
@param ResetStatus  リセットのステータスコードを指定。システムのリセットが正常な動作ならEFI_SUCCESSを指定
@param DataSize     ResetDataのデータサイズをバイト単位で指定
@param ResetData    ResetStatusがEFI_SUCCESSでないなら、NULL終端文字列を指定することで呼び出し元へ伝達できるらしい。
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_RESET_SYSTEM) (
    IN EFI_RESET_TYPE   ResetType,
    IN EFI_STATUS       ResetStatus,
    IN UINTN            DataSize,
    IN VOID             *ResetData OPTIONAL
);

typedef struct {
    char _pad1[96];
    
    // Miscellaneous Services
    char _pad2[8];
    EFI_RESET_SYSTEM ResetSystem;

} EFI_RUNTIME_SERVICES;

// EFI_SYSTEM_TABLE
typedef struct{
    char _pad1[44];
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
    char _pad2[8];
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    char _pad3[16];
    EFI_RUNTIME_SERVICES *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices;
} EFI_SYSTEM_TABLE;

#endif