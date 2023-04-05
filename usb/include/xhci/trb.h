#ifndef __TRB_H__
#define __TRB_H__

#include <stdint.h>

#include <macros.h>

// TRB Types defined on p.511
typedef enum {
    Reserved,
    Normal,
    SetupStage,
    DataStage,
    StatusStage,
    Isoch,
    Link,
    EventData,
    NoOp,
    EnableSlotCommand,
    DisableSlotCommand,
    AddressDeviceCommand,
    ConfigureEndpointCommand,
    EvaluateContextCommand,
    ResetEndpointCommand,
    StopEndpointCommand,
    SetTRDequeuePointerCommand,
    ResetDeviceCommand,
    ForceEventCommand,
    NegotiateBandnwidthCommand,
    SetLatencyToleranceValueCommand,
    GetPortBandwidthCommand,
    ForceHeaderCommand,
    NoOpCommand,
    GetExtendedPropertyCommand,
    SetExtendedPropertyCommand,
    // 26-31 Reserved
    TransferEvent = 32,
    CommandCompletionEvent,
    PortStatsChangeEvent,
    BandwidthRequestEvent,
    DoorbellEvent,
    HostControllerEvent,
    DeviceNotificationEvent,
    MFINDEXWrapEvent,
    // 40-47 Reserved
    // 48-63 VendorDefined 
} TRBType;

// TRB Completion Codes defined on p.507
typedef enum {
    Invalid,
    Success,
    DataBufferError,
    BabbleDetectedError,
    USBTransactionError,
    TRBError,
    StallError,
    ResourceError,
    BandwidthError,
    NoSlotsAvailableError,
    InvalidStreamTypeError,
    SlotNotEnabledError,
    EndpointNotEnabledError,
    ShortPacket,
    RingUnderrun,
    RingOverrun,
    VFEventRingFullError,
    ParameterError,
    BandwidthOverrunError,
    ContextStateError,
    NoPingResponseError,
    EventRingFullError,
    IncompatibleDeviceError,
    MissedServiceError,
    CommandRingStoppend,
    CommandRingAborted,
    Stopped,
    StoppedLengthInvalid,
    StoppedShortPacket,
    MaxExitLatencyTooLargeError,
    // 30 Reserved
    IsochBufferOverrun = 31,
    EventLostError,
    UndefinedError,
    InvalidStreamIdError,
    SecondaryBandwidthError,
    SplitTransactionError,
    // 37-191 Reserved
    // 192-223 VendorDefinedError
    // 224-255 VenforDefineInfo
} TRBCompletionCodes;

typedef struct __attribute__((packed)) {
    uint64_t    Parameter   : 64;
    uint32_t    Status      : 32;
    uint8_t     C           : 1;
    uint16_t    Rsvd        : 9;
    uint8_t     TRBType     : 6;
    uint16_t    Control     : 16; 
} TRB;

// Command TRBs defined on p.487
typedef struct __attribute__((packed)){
    uint32_t    Rsvd[3];
    uint8_t     C       : 1;
    uint16_t    Rsvd    : 9;
    uint8_t     TRBType : 6;
    uint16_t    Rsvd    : 16;
} NoOpCommandTRB;

// Event TRBs defined on p.477
typedef struct __attribute__((packed)) {
    uint8_t     Rsvd                        : 4;
    uint64_t    CommandTRBPointerHiandLo    : 60;
    uint32_t    CommandCompletionParameter  : 24;
    uint8_t     CompletionCode              : 8;
    uint8_t     C                           : 1;
    uint16_t    Rsvd                        : 9;
    uint8_t     TRBType                     : 6;
    uint8_t     VFId                        : 8;
    uint8_t     SlotId                      : 8;
} CommandCompletionEventTRB;

// Other TRBs defined on p.503

// Link TRB defined on p.503
typedef struct __attribute__((packed)) {
    uint8_t     Rsvd                        : 4;
    uint64_t    RingSegmentPointerHiandLo   : 60;
    uint32_t    Rsvd                        : 22;
    uint16_t    InterrupterTarget           : 10;
    uint8_t     C                           : 1;
    uint8_t     TC                          : 1;
    uint8_t     Rsvd                        : 2;
    uint8_t     CH                          : 1;
    uint8_t     IOC                         : 1;
    uint8_t     Rsvd                        : 4;
    uint8_t     TRBType                     : 6;
    uint16_t    Rsvd                        : 16;
} LinkTRB;

#endif 