#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <segment.h>
#include <macros.h>
#include <asmfunc.h>

#include <stdint.h>

typedef enum {
    xHCIVector = 0x20,
} InterruptVector;

// 64-Bit IDT Gate Descriptors defined on p.3024 of Intel® 64 and IA-32 Architectures Software Developer’s Manual Vol.3A
typedef union __attribute__((packed)) {
    uint16_t    data;
    struct __attribute__((packed)) {
        uint8_t     IST     : 3; // Interrupt Stack Table
        uint8_t     Rsvd    : 5;
        uint8_t     Type    : 4;
        uint8_t     Rsvd    : 1;
        uint8_t     DPL     : 2; // Descriptor Privilege Level
        uint8_t     P       : 1; // Segment Present Flag
    } bits; 
} InterruptDescriptorAttribute;

typedef struct __attribute__((packed)) {
    uint16_t                        offsetLow;
    SegmentSelector                 segmentSelector;
    InterruptDescriptorAttribute    attr;
    uint16_t                        offsetMiddle;
    uint32_t                        offsetHigh;
    uint32_t                        Rsvd;
} InterruptDescriptor;

// MikanOSを参考に追加。なぜ必要かは良く分かってない。
typedef struct {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} InterruptFrame;

void setIDTEntry(InterruptVector id, uint64_t offset);
void loadIDT(void);

#endif 