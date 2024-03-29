#include <xhci/registers.h>

CapabilityRegistes      *cap    = NULL;
OperationalRegisters    *op     = NULL;
InterrupterRegisterSet  *intr   = NULL;
DoorBellRegister        *db     = NULL;
PortRegisterSet         *pr     = NULL;

void saveRegs(void) {
    uint64_t mmioBase = readBAR(xhcDev.bus, xhcDev.dev, xhcDev.func);
    cap     = (CapabilityRegistes *)mmioBase;
    op      = (OperationalRegisters *)(mmioBase + cap->CAPLENGTH);
    intr    = (InterrupterRegisterSet *)(mmioBase + (cap->RTSOFF.bits.RuntimeRegisterSpaceOffset << 5) + 0x20u);
    db      = (DoorBellRegister *)(mmioBase + (cap->DBOFF.bits.DoorBellArrayOffset << 2));
    pr      = (PortRegisterSet *)((uint64_t)op + 0x400);

    Log(
        Debug,
        "cap: %#x op: %#x intr: %#x db: %#x pr: %#x\n",
        cap,
        op,
        intr,
        db,
        pr
    );
}