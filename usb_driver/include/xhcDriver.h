#ifndef __XHC_DRIVER_H__
#define __XHC_DRIVER_H__

#include <pci.h>
#include <usbError.h>
#include <stdalign.h>
#include <macros.h>
#include <devContext.h>
#include <ring.h>

typedef struct {
    uint8_t bus;
    uint8_t dev;
    uint8_t func;
} Device;

typedef struct __attribute__((packed)) {
    uint8_t     CAPLENGTH;
    uint8_t     Rsvd;
    uint16_t    HCIVERSION;
    struct __attribute__((packed)) {
        uint8_t     MaxSlots    : 8;
        uint16_t    MaxIntrs    : 11;
        uint16_t    Rsvd        : 5;
        uint16_t    MaxPorts    : 8;
    } HCSPARAMS1;
    uint32_t    HCSPARAMS2;
    uint32_t    HCSPARAMS3;
    uint32_t    HCCPARAMS1;
    uint32_t    DBOFF;
    uint32_t    RTSOFF;
    uint32_t    HCCPARAMS2;
    // Rsvd (CAPLENTH - 20h bytes)
} CapabilityRegistes;

typedef struct __attribute__((packed)) {
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
    } USBCMD;
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
    } USBSTS;
    struct __attribute__((packed)) {
        uint16_t    PageSize    : 16;
        uint16_t    Rsvd        : 16;
    } PAGESIZE;
    uint8_t     Rsvd;
    struct __attribute__((packed)) {
        uint16_t    NotificationEnable  : 16;
        uint16_t    Rsvd                : 16;
    } DNCTRL;
    struct __attribute__((packed)) {
        uint8_t     RCS                 : 1;
        uint8_t     CS                  : 1;
        uint8_t     CA                  : 1;
        uint8_t     CRR                 : 1;
        uint8_t     Rsvd                : 2;
        uint64_t    CommandRingPointer  : 58;
    } CRCR;
    uint16_t    Rsvd;
    struct __attribute__((packed)) {
        uint8_t     Rsvd                                    : 6;
        uint64_t    DeviceContextBaseAddressArrayPointer    : 58;
    } DCBAAP;
    struct __attribute__((packed)) {
        uint8_t     MaxSlotsEn  : 8;
        uint8_t     U3E         : 1;
        uint8_t     CIE         : 1;
        uint32_t    Rsvd        : 22;
    } CONFIG;
} OperationalRegisters;

extern Device xhcDev;

UsbError initXhc(int NumDevice);

#endif 