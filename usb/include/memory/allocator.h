#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <xhci/registers.h>
#include <kstdio.h>
#include <kstring.h>

#include <stdint.h>
#include <stdalign.h>
#include <stddef.h>

uintptr_t AllocMem(uint64_t size, uint64_t align, uint64_t boundary);

#endif 