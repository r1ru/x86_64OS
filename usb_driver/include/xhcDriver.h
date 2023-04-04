#ifndef __XHC_DRIVER_H__
#define __XHC_DRIVER_H__

#include <pci.h>
#include <usbError.h>
#include <devContext.h>
#include <registers.h>
#include <commandRing.h>
#include <eventRing.h>
#include <interrupt.h>

#include <stdalign.h>

UsbError initXhc(int NumDevice);

#endif 