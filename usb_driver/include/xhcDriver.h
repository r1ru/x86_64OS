#ifndef __XHC_DRIVER_H__
#define __XHC_DRIVER_H__

#include <pci.h>
#include <usbError.h>
#include <stdalign.h>
#include <macros.h>
#include <devContext.h>
#include <ring.h>
#include <trb.h>
#include <registers.h>
#include <commandRing.h>

typedef struct {
    uint8_t bus;
    uint8_t dev;
    uint8_t func;
} Device;

extern Device xhcDev;

UsbError initXhc(int NumDevice);

#endif 