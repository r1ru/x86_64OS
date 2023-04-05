#ifndef __XHC_DRIVER_H__
#define __XHC_DRIVER_H__

#include <stdalign.h>

#include <usbError.h>
#include <pci/pci.h>
#include <xhci/devContext.h>
#include <xhci/registers.h>
#include <xhci/commandRing.h>
#include <xhci/eventRing.h>
#include <interrupt.h>

UsbError initXhc(int NumDevice);

#endif 