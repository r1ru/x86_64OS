#include <xhciInterruptHandler.h>

__attribute__((interrupt)) void xhciInterruptHandler(InterruptFrame *frame) {
    pushMsg((InterruptMessage){.ty = InterruptXHCI});
    // write to End Of Interrupt Register
    *((uint32_t *)0xfee000b0) = 0;
}