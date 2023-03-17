#ifndef __ASM_FUNC_H__
#define __ASM_FUNC_H__

#include <stdint.h>

void ioOut32(uint16_t addr, uint32_t data);
uint32_t ioIn32(uint16_t addr);

#endif 