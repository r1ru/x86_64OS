#include <device/descriptor.h>

static char* type2str(uint8_t ty) {
    switch(ty) {
        case DEVICE:
            return "DEVICE";
        case CONFIGURATION:
            return "CONFIGURATION";
        case STRING:
            return "STRING";
        case INTERFACE:
            return "INTERFACE";
        case ENDPOINT:
            return "ENDPOINT";
        case INTERFACE_POWER:
            return "INTERFACE_POWER";
        case OTG:
            return "OTG";
        case DEBUG:
            return "DEBUG";
        case INTERFACE_ASSOCIATION:
            return "INTERFACE_ASSOCIATION";
        case BOS:
            return "BOS";
        case DEVICE_CAPABILITY:
            return "DEVICE_CAPABILITY";
        case SUPERSPEED_USB_ENDPOINT_COMPANION:
            return "SUPERSPEED_USB_ENDPOINT_COMPANION";
        default:
            return ""; // unreachable
    }
}

// TODO: エラー処理
void PrintDeviceDescriptor(int slotID) {
    USBDevice *dev = GetDeviceBySlotID(slotID);
    StandardDeviceDescriptor* dd = &dev->DeviceDescriptor;

    Log(
        Info,
        "[*] Device Descriptor:\n"
    );

    Log(
        Info,
        "    bLength: %#x, bNumConfigurations: %#x\n",
        dd->hdr.bLength,
        dd->bNumConfigurations
    );
}
// TODO: エラー処理
void PrintConfigurationDescriptor(int slotID) {
    USBDevice *dev = GetDeviceBySlotID(slotID);
    StandardConfigurationDescripotr *cd = (StandardConfigurationDescripotr *)dev->Configuration;

    uint16_t total = cd->wTotalLength;
    Log(
        Info,
        "[*] Configuration Descriptor(wTotalLength: %#x, bNumInterfaces: %#x)\n",
        total,
        cd->bNumInterfaces
    );
    
    int16_t remain = total - cd->hdr.bLength;
    DescriptorHeader *hdr = (DescriptorHeader *)((uint8_t *)cd + cd->hdr.bLength);

    while(0 < remain) {
        Log(
            Info,
            "    %#x %s\n",
            hdr->bLength,
            type2str(hdr->bDescriptorType)
        );

        remain -= hdr->bLength;
        hdr = (DescriptorHeader *)((uint8_t *)hdr + hdr->bLength);
    }
}