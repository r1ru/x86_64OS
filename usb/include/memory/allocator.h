#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <stdint.h>
#include <stdalign.h>
#include <stddef.h>

#include <logger.h>
#include <kstring.h>
#include <xhci/registers.h>

uintptr_t AllocMem(uint64_t size, uint64_t align, uint64_t boundary);

#endif 