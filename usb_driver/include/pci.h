#ifndef __PCI_H__
#define __PCI_H__

#include <asmfunc.h>
#include <xhcDriver.h>
#include <kstdio.h>

#include <stdint.h>
#include <stdbool.h>

uint32_t makeAddress(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg_offset);
void writeData(uint32_t address, uint32_t data);
uint32_t readData(uint32_t address);

int scanAllBus(void);

#endif 