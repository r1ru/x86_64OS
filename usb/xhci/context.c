#include <xhci/context.h>

// dcabaの配列
DeviceContext** dcbaa;

USBError initDCBAA(int maxSlotsEn) {
    // xHCが扱えるデバイスコンテキストの数の最大値を設定する
    printk("MaxSlots: %#x\n", cap->HCSPARAMS1.bits.MaxSlots);
    CONFIGBitmap config = op->CONFIG;
    config.bits.MaxSlotsEn = maxSlotsEn;
    op->CONFIG = config;

    // SlotIDは1始まりなので+1する
    dcbaa = (DeviceContext **)AllocMem(sizeof(DeviceContext *) * (maxSlotsEn + 1), 64, 0);
    if(!dcbaa)
        return NewErrorf(ErrMemory, "could not allocate DCBAA");
    
    op->DCBAAP = (DCBAAPBitmap) {
        .bits.DeviceContextBaseAddressArrayPointer = (uint64_t)dcbaa >> 6
    };

    return Nil;
}

// TODO: PAGESIZEを4096固定でなくする
DeviceContext * newDeviceContext(void) {
    return (DeviceContext *)AllocMem(sizeof(DeviceContext), 64, 4096);
}

InputContext * newInputContext(void) {
    return (InputContext *)AllocMem(sizeof(InputContext), 64, 4096);
}