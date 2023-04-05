#include <xhci/driver.h>

// とりあえず要素は固定(TODO: malloc作る？)
alignas(64) DeviceContext* dcabaa[64];

UsbError initXhc(int NumDevice) {
    saveRegs();

    // xHCの初期化
    if(!op->USBSTS.bits.HCH)
        return xHCNotHalted;

    USBCMDBitmap usbcmd = (USBCMDBitmap)op->USBCMD.data;
    usbcmd.bits.HCRST = 1;
    op->USBCMD.data = usbcmd.data;
    while(op->USBCMD.bits.HCRST);
    while(op->USBSTS.bits.CNR);
    printk("xHC reset completed\n");

    // xHCが扱えるデバイスコンテキストの数の最大値を設定する
    printk("MaxSlots: %#x\n", cap->HCSPARAMS1.bits.MaxSlots);
    CONFIGBitmap config = (CONFIGBitmap)op->CONFIG.data;
    config.bits.MaxSlotsEn = NumDevice;
    op->CONFIG.data = config.data;
    op->DCBAAP.data = (uint64_t)dcabaa;

    // Command Ringの設定
    initCommandRing();
    printk("command ring setup completed\n");

    // Event Ringの設定(Primary Interrupter)
    initEventRing();
    printk("event ring setup completed\n");

    // 割り込みの設定(Primary Interrupter)
    IMODBitmap imod = (IMODBitmap)intr->IMOD.data;
    IMANBitmap iman = (IMANBitmap)intr->IMAN.data;
    imod.bits.IMODI = 4000;
    iman.bits.IP = 1;
    iman.bits.IE = 1;
    intr->IMOD.data = imod.data;
    intr->IMAN.data = iman.data;
    usbcmd = (USBCMDBitmap)op->USBCMD.data;
    usbcmd.bits.INTE = 1;
    op->USBCMD.data = usbcmd.data;
    MessageAddressBitmap    msgAddr = {
        .bits = {
            .DM = 0,
            .RH = 0,
            .DestinationId = (uint8_t)(*((uint32_t *)0xfee00020) >> 24),
            .OFEEH = 0xfeeu
        }
    };
    MessageDataBitmap       msgData = {
        .bits = {
            .TM = 1,
            .LV = 1,
            .DM = 0,
            .Vector = xHCIVector
        }
    };
    configureMSI(xhcDev, msgAddr, msgData);

    // start xHC 
    usbcmd = (USBCMDBitmap)op->USBCMD.data;
    usbcmd.bits.R_S = 1;
    op->USBCMD.data = usbcmd.data;
    while(op->USBSTS.bits.HCH);
    printk("xHC started\n");

    return xHCSetupCompleted;
}
