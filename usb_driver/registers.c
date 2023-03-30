#include <registers.h>

CapabilityRegistes      *cap    = NULL;
OperationalRegisters    *op     = NULL;
InterrupterRegisterSet  *intr   = NULL;
DoorBellRegister        *db     = NULL;

void saveRegs(void) {
    uint64_t mmioBase = readBAR(xhcDev.bus, xhcDev.dev, xhcDev.func);
    CapabilityRegistes *capRegs = (CapabilityRegistes *)mmioBase;
    cap     = capRegs;
    op      = (OperationalRegisters *)(mmioBase + capRegs->CAPLENGTH);
    intr    = (InterrupterRegisterSet *)(mmioBase + capRegs->RTSOFF + 0x20);
    db      = (DoorBellRegister *)(mmioBase + capRegs->DBOFF);
    printk(
        "cap: %#x op: %#x intr: %#x db: %#x\n",
        cap,
        op,
        intr,
        db
    );
}