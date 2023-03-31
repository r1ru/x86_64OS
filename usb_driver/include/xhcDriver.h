#ifndef __XHC_DRIVER_H__
#define __XHC_DRIVER_H__

#include <pci.h>
#include <usbError.h>
#include <stdalign.h>
#include <macros.h>
#include <devContext.h>
#include <trb.h>
#include <registers.h>
#include <commandRing.h>
#include <eventRing.h>

UsbError initXhc(int NumDevice);

#endif 