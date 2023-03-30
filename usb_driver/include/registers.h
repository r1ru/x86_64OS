#ifndef __REGISTERS_H__
#define __REGISTERS_H__

#include <stdint.h>
#include <stddef.h>
#include <kstdio.h>
#include <pci.h>

// Software shoud write following registers using unly "Dword Access"
// If the register is 64-bit, software can also use "Qword Access" 

// Capability Registers defined on p.381

// HCSPARAMS1 defined on p.382
typedef union {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint8_t     MaxSlots    : 8;
        uint16_t    MaxIntrs    : 11;
        uint16_t    Rsvd        : 5;
        uint16_t    MaxPorts    : 8;
    } bits;
} HCSPARAMS1Bitmap;

typedef struct __attribute__((packed)) {
    uint8_t             CAPLENGTH;
    uint8_t             Rsvd;
    uint16_t            HCIVERSION;
    HCSPARAMS1Bitmap    HCSPARAMS1;
    uint32_t            HCSPARAMS2;
    uint32_t            HCSPARAMS3;
    uint32_t            HCCPARAMS1;
    uint32_t            DBOFF;
    uint32_t            RTSOFF;
    uint32_t            HCCPARAMS2;
    // Rsvd (CAPLENTH - 20h bytes)
} CapabilityRegistes;

// Operational Registers defined on p.391

// USBCMD defined on p.393
typedef union {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint8_t     R_S     : 1;
        uint8_t     HCRST   : 1;
        uint8_t     INTE    : 1;
        uint8_t     HSEE    : 1;
        uint8_t     Rsvd    : 3;
        uint8_t     LHCRST  : 1;
        uint8_t     CSS     : 1;
        uint8_t     CRS     : 1;
        uint8_t     EWE     : 1;
        uint8_t     EU3S    : 1;
        uint8_t     Rsvd    : 1;
        uint8_t     CME     : 1;
        uint8_t     ETE     : 1;
        uint8_t     TSC_EN  : 1;
        uint8_t     VTIOE   : 1;
        uint16_t    Rsvd    : 15;
    } bits;
} USBCMDBitmap;

// USBSTS defined on p.397
typedef union {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint8_t     HCH     : 1;
        uint8_t     Rsvd    : 1;
        uint8_t     HSE     : 1;
        uint8_t     EINT    : 1;
        uint8_t     PCD     : 1;
        uint8_t     Rsvd    : 3;
        uint8_t     SSS     : 1;
        uint8_t     RSS     : 1;
        uint8_t     SRE     : 1;
        uint8_t     CNR     : 1;
        uint8_t     HCE     : 1;
        uint32_t    Rsvd    : 19;
    } bits;
} USBSTSBitmap;

// PAGESIZE defined on p.399
typedef union {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint16_t    PageSize    : 16;
        uint16_t    Rsvd        : 16;
    } bits;
} PAGESIZEBitmap;

// DNCTRL defined on p.400
typedef union {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint16_t    NotificationEnable  : 16;
        uint16_t    Rsvd                : 16;
    } bits;
} DNCTRLBitmap;

// CRCR defined on p.401
typedef union {
    uint64_t    data;
    struct __attribute__((packed)) {
        uint8_t     RCS                 : 1;
        uint8_t     CS                  : 1;
        uint8_t     CA                  : 1;
        uint8_t     CRR                 : 1;
        uint8_t     Rsvd                : 2;
        uint64_t    CommandRingPointer  : 58;
    } bits;
} CRCRBitmap;

// DCBAAP defined on p.403
typedef union {
    uint64_t    data;
    struct __attribute__((packed)) {
        uint8_t     Rsvd                                    : 6;
        uint64_t    DeviceContextBaseAddressArrayPointer    : 58;
    } bits;
} DCBAAPBitmap;

// CONFIG defined on p.404
typedef union {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint8_t     MaxSlotsEn  : 8;
        uint8_t     U3E         : 1;
        uint8_t     CIE         : 1;
        uint32_t    Rsvd        : 22;
    } bits;
} CONFIGBitmap;

typedef struct __attribute__((packed)) {
    USBCMDBitmap    USBCMD;
    USBSTSBitmap    USBSTS;
    PAGESIZEBitmap  PAGESIZE;
    uint64_t        Rsvd;
    DNCTRLBitmap    DNCTRL;
    CRCRBitmap      CRCR;
    uint64_t        Rsvd[2];
    DCBAAPBitmap    DCBAAP;
    CONFIGBitmap    CONFIG;
} OperationalRegisters;

// Interrupter Reigster Set defined on p.424

// IMAN defined on p.425
typedef union {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint8_t     IP      : 1;
        uint8_t     IE      : 1;
        uint32_t    Rsvd    : 30;   
    } bits;
} IMANBitmap;

// IMOD defined on p.426
typedef union {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint16_t    IMODI   : 16;
        uint16_t    IMODC   : 16;
    } bits;
} IMODBitmap;

// Event Ring Registers defined on p.427

// ERSTSZ defined on p.427
typedef union {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint16_t    EventRingSegmentTableSize   : 16;
        uint16_t    Rsvd                        : 16;
    } bits;
} ERSTSZBitmap;

// ERSTBA defined on p.427
typedef union {
    uint64_t    data;
    struct __attribute__((packed)) {
        uint8_t     Rsvd                                        : 6;
        uint64_t    EventRingSegmentTableBaseAddressRegister    : 58;
    } bits;
} ERSTBABitmap;

// ERDP defined on p.428
typedef union {
    uint64_t    data;
    struct __attribute__((packed)) {
        uint8_t     DESI                    : 3;
        uint8_t     EHB                     : 1;
        uint64_t    EventRingDequeuePointer : 60;
    } bits;
} ERDPBitmap;

typedef struct __attribute__((packed)) {
    IMANBitmap      IMAN;
    IMODBitmap      IMOD;
    ERSTSZBitmap    ERSTSZ;
    uint32_t        Rsvd;
    ERSTBABitmap    ERSTBA;
    ERDPBitmap      ERDP;
} InterrupterRegisterSet;

// Doorbell Register defined on p.429
// Software should "read" and write these registers using only "Dword Access"
typedef union{
    uint32_t data;
    struct __attribute__((packed)) {
        uint8_t     DBTarget    : 8;
        uint8_t     Rsvd        : 8;
        uint16_t    DBStreamId  : 16;
    } bits;
} DoorBellRegister;

// holds a pointer to each register as a global variable
// these pointer can be used after saveRegs called
extern CapabilityRegistes       *cap;
extern OperationalRegisters     *op;
extern InterrupterRegisterSet   *intr;
extern DoorBellRegister         *db;

void saveRegs(void);

#endif 