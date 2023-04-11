#ifndef __XHC_DRIVER_H__
#define __XHC_DRIVER_H__

#include <stdbool.h>

#include <usbError.h>
#include <pci/pci.h>
#include <xhci/context.h>
#include <xhci/registers.h>
#include <xhci/txRing.h>
#include <xhci/rxRing.h>
#include <xhci/trb.h>
#include <xhci/context.h>
#include <port/manager.h>
#include <interrupt.h>
#include <memory/allocator.h>

UsbError initXhc(int NumDevice);
void ProcessEvents(void);

#endif 