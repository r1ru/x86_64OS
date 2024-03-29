#ifndef __PCI_H__
#define __PCI_H__

#include <stdint.h>
#include <stdbool.h>

#include <pci/asmfunc.h>
#include <logger.h>
#include <macros.h>

typedef struct {
    uint8_t bus;
    uint8_t dev;
    uint8_t func;
} Device;

// xhcDev can be used after scanAllBus called
extern Device xhcDev;

// MSI Capability defined on p.232 of PCI_SPEC_V3_0
// There are four structures, and Message Controll can be used to determine which one to use
// In the following, the structure is expected to be 64-bit Message Address and Per-vector Masking
// Message Address and Message Data formats are architecture-dependent
// In the following, format for x86_64 is expected (defined on p.3225 of Intel® 64 and IA-32 Architectures Software Developer’s Manual Vol.3A)
typedef union __attribute__((packed)){
    uint32_t    data;
    struct __attribute__((packed)){
        uint8_t     CapId                   : 8;
        uint8_t     NxtPtr                  : 8;
        // Message Control
        uint8_t     MSIEnable               : 1;
        uint8_t     MultipleMessageCapable  : 3;
        uint8_t     MultipleMessageEnable   : 3;
        uint8_t     Addr64Capable           : 1;
        uint8_t     PreVectorMaskingCapable : 1;
        uint8_t     Rsvd                    : 7;
    } bits;
} HeaderBitmap;

typedef union __attribute__((packed)) {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint8_t     XX              : 2;
        uint8_t     DM              : 1;
        uint8_t     RH              : 1;
        uint8_t     Rsvd            : 8;
        uint8_t     DestinationId   : 8;
        uint16_t    OFEEH           : 12;
    } bits;
} MessageAddressBitmap;

typedef union __attribute__((packed)) {
    uint32_t    data;
    struct __attribute__((packed)) {
        uint8_t     Vector              : 8;
        uint8_t     DM                  : 3; // Delivery Mode
        uint8_t     Rsvd                : 3;
        uint8_t     LV                  : 1; // Level Trigger Mode
        uint8_t     TM                  : 1; // Trigger Mode
        uint16_t    Rsvd                : 16; 
    } bits;
} MessageDataBitmap;

typedef struct  __attribute__((packed)) {
    HeaderBitmap            header;
    MessageAddressBitmap    MessageAddress;
    uint32_t                MessageUpperAddress;
    MessageDataBitmap       MessageData;
    uint32_t                MaskBits;
    uint32_t                PendingBits;
} MSICapability;

uint64_t readBAR(uint8_t bus, uint8_t dev, uint8_t func);
int scanAllBus(void);

void configureMSI(Device dev, MessageAddressBitmap msgAddr, MessageDataBitmap msgData);

#endif 