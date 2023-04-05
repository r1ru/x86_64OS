#ifndef __XHCI_INTERRUPT_HANDLER_H__
#define __XHCI_INTERRUPT_HANDLER_H__

#include <interrupt.h>
#include <interruptMessageQueue.h>

__attribute__((interrupt)) void xhciInterruptHandler(InterruptFrame *frame);

#endif 