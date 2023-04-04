#include <segment.h>

static const SegmentDescriptor gdt[3] = {
    [0] = {0},
    [1] = {
        .bits = {
            // Executed/Read
            .Type   = 10,
            .S      = 1,
            .DPL    = 0,
            .P      = 1,
            .L      = 1,
            .DB     = 0
        }
    },
    [2] = {
        .bits = {
            // Read-Only, accessed
            .Type   = 2,
            .S      = 1,
            .DPL    = 0,
            .P      = 1,
            .L      = 1,
            .DB     = 0
        }
    }
};

void initSegmentation(void) {
    SegmentSelector cs = {
        .bits = {
            .Index = 1
        }
    };
    SegmentSelector ss = {
        .bits = {
            .Index = 2
        }
    };

    loadGDT(sizeof(gdt) - 1, (uint64_t)gdt);
    setCSSS(cs.data, ss.data);
}
