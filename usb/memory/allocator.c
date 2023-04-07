#include <memory/allocator.h>

#define POOLSIZE    (4096 * 32)

static alignas(4096) uint8_t  pool[POOLSIZE];
static uintptr_t ptr = (uintptr_t)pool;

#define _align(_addr, _align) ((_addr + _align - 1) & ~(_align - 1))

uintptr_t AllocMem(uint64_t size, uint64_t align, uint64_t boundary) {
    if(align) {
        ptr = _align(ptr, align);
    }
    
    if(boundary) {
        uintptr_t nxtBoundary = _align(ptr, boundary);
        if(nxtBoundary < ptr + size)
            ptr = nxtBoundary;
    }

    if((uintptr_t)pool + POOLSIZE < ptr + size) 
        return (uintptr_t)NULL;
    
    uintptr_t p = ptr;
    ptr += size;

    printk("alloc mem@%p size: %#x\n", p, size);
    
    return p;
}