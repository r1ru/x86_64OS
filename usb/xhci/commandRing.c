#include <xhci/commandRing.h>

static alignas(64) CommandRing cr;

void initCommandRing(void) {
    cr.PCS      = 1;
    cr.writeIdx = 0;
    CRCRBitmap crcr = op->CRCR;
    crcr.bits.CommandRingPointer = (uint64_t)cr.buf >> 6;
    crcr.bits.RCS = cr.PCS;
    op->CRCR = crcr;
    // cr.bufの最後にLinkTRBを配置
    LinkTRB *link = (LinkTRB *)&cr.buf[CRSIZE - 1];
    *link = (LinkTRB) {
        .RingSegmentPointerHiandLo  = (uint64_t)cr.buf >> 4,
        .TRBType                    = Link,
        .TC                         = 1,
        .C                          = cr.PCS
    };
}

CommandRingError pushCommand(TRBType ty) {
    NoOpCommandTRB *noOp = NULL; // TODO: TRBをunion型にする？
    switch(ty) {
        case NoOpCommand:
            // compund literalを使うとundefined symbol: memsetが出た。-ffreestanding指定なんだけどな...
            noOp = (NoOpCommandTRB *)&cr.buf[cr.writeIdx++];
            kmemset(noOp, 0, sizeof(NoOpCommandTRB));
            noOp->TRBType = NoOpCommand;
            noOp->C = cr.PCS;
            printk("NoOpCommand@%p\n", noOp);
            break;
        default:
            return InvalidCommand;
    }
    if (cr.writeIdx == CRSIZE - 1) {
        LinkTRB *link = (LinkTRB *)&cr.buf[cr.writeIdx];
        link->C = cr.PCS;
        cr.PCS = !cr.PCS;
        cr.writeIdx = 0;
    }
    // Ring Doorbell
    db->data = 0;
    return CommandRequested;
}



