#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <graphics.h>
#include <console.h>
#include <segment.h>
#include <interruptMessageQueue.h>
#include <usbError.h>
#include <pci/pci.h>
#include <xhci/driver.h>
#include <xhci/interruptHandler.h>
#include <xhci/txRing.h>
#include <xhci/rxRing.h>
#include <xhci/trb.h>

typedef void EntryPointType(FrameBufferInfo *info);

#endif 