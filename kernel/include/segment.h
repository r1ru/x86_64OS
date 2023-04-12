#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <asmfunc.h>

#include <stdint.h>

// Segment Selector defined on p.2905 of Intel® 64 and IA-32 Architectures Software Developer’s Manual Vol.3A
typedef union __attribute__((packed)) {
    uint16_t    data;
    struct __attribute__((packed)) {
        uint8_t     RPL     : 2; // Requested Privillege Level(can range from 0 to 3)
        uint8_t     TI      : 1; // Table Indicator 0 = GDT, 1=LDT
        uint16_t    Index   : 13;
    } bits;
} SegmentSelector;

// Segment Descriptor defined on p.2908
// Limit and Base are ignored in 64-bit mode
// Descriptor type is determined by Type and S See p.2910 for details.
typedef union __attribute__((packed)) {
    uint64_t data;
    struct __attribute__((packed)) {
        uint16_t    LimitLow    : 16;
        uint16_t    BaseLow     : 16;
        uint8_t     BaseMiddle  : 8;
        uint8_t     Type        : 4;
        uint8_t     S           : 1; // Descriptor Type 0=system 1=code of data
        uint8_t     DPL         : 2; // Descriptor Privillege Level
        uint8_t     P           : 1; // Segment Present
        uint8_t     LimitHigh   : 4;
        uint8_t     AVL         : 1; // Availlable for use by system software
        uint8_t     L           : 1; // 64-bit code segment
        uint8_t     DB          : 1; // Default operation size
        uint8_t     G           : 1; // Granularity
        uint8_t     BaseHigh    : 8;
    } bits;
} SegmentDescriptor;

void initSegmentation(void);

#endif