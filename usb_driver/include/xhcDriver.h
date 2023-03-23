#ifndef __XHC_DRIVER_H__
#define __XHC_DRIVER_H__

#include <pci.h>

typedef struct {
    uint8_t bus;
    uint8_t dev;
    uint8_t func;
} Device;

extern Device xhcDev;

#endif 