#ifndef __PCI_H__
#define __PCI_H__

#include <asmfunc.h>
#include <xhcDriver.h>
#include <kstdio.h>

#include <stdint.h>
#include <stdbool.h>

uint64_t readBAR(uint8_t bus, uint8_t dev, uint8_t func);
int scanAllBus(void);

#endif 