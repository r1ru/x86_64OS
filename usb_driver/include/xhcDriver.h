#ifndef __XHC_DRIVER_H__
#define __XHC_DRIVER_H__

#include <pci.h>

typedef struct {
    uint8_t bus;
    uint8_t dev;
    uint8_t func;
} Device;

typedef struct __attribute__((packed)) {
    uint8_t     CAPLENGTH;
    uint8_t     ;
    uint16_t    HCIVERSION;
    uint32_t    HCSPARAMS1;
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
        uint8_t R_S     : 1;
        uint8_t HCRST   : 1;
        uint8_t INTE    : 1;
        uint8_t HSEE    : 1;
        uint8_t         : 3; // RsvdZ
        uint8_t LHCRST  : 1;
        uint8_t CSS     : 1;
        uint8_t CRS     : 1;
        uint8_t EWE     : 1;
        uint8_t EU3S    : 1;
        uint8_t         : 1; // RsvdP
        uint8_t CME     : 1;
        uint8_t ETE     : 1;
        uint8_t TSC_EN  : 1;
        uint8_t VTIOE   : 1;
        uint16_t        : 15; // RsvdP
    } USBCMD;
    struct __attribute__((packed)) {
        uint8_t HCH     : 1;
        uint8_t         : 1; // RsvdZ
        uint8_t HSE     : 1;
        uint8_t EINT    : 1;
        uint8_t PCD     : 1;
        uint8_t         : 3; // RsvdZ
        uint8_t SSS     : 1;
        uint8_t RSS     : 1;
        uint8_t SRE     : 1;
        uint8_t CNR     : 1;
        uint8_t HCE     : 1;
        uint32_t        : 19; // RsvdZ
    } USBSTS;
} OperationalRegisters;

extern Device xhcDev;

int initXhc(void);

#endif 