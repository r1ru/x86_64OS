#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <graphics.h>
#include <kstdio.h>
#include <console.h>
#include <pci.h>
#include <usbError.h>
#include <segment.h>
#include <xhciInterruptHandler.h>
#include <commandRing.h>
#include <interruptMessageQueue.h>
#include <trb.h>

typedef void EntryPointType(FrameBufferInfo *info);

#endif 