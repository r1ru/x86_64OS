#ifndef __ASM_FUNC_H__
#define __ASM_FUNC_H__

#include <stdint.h>

void loadGDT(uint16_t limit, uint64_t offset);
void setCS(uint16_t ss);
uint16_t getCS(void);
void _loadIDT(uint16_t limit, uint64_t offset);

#endif 