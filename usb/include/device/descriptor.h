#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

#include <stdint.h>
#include <logger.h>

// DescriptorTypes defined on p.9-17 of USB3.0 Spec
typedef enum {
    DEVICE                              = 1,
    CONFIGURATION                       = 2,
    STRING                              = 3,
    INTERFACE                           = 4,
    ENDPOINT                            = 5,
    INTERFACE_POWER                     = 8,
    OTG                                 = 9,
    DEBUG                               = 10,
    INTERFACE_ASSOCIATION               = 11,
    BOS                                 = 15,
    DEVICE_CAPABILITY                   = 16,
    SUPERSPEED_USB_ENDPOINT_COMPANION   = 48
} DesctiprotTypes;

typedef struct __attribute__((packed)) {
    uint8_t     bLength;
    uint8_t     bDescriptorType;    
} DescriptorHeader;

// StandardDeviceDescriptor defined on p.9-31
typedef struct __attribute__((packed)) {
    DescriptorHeader    hdr;
    uint16_t            cdUSB;
    uint8_t             bDeviceClass;
    uint8_t             bDeviceSubClass;
    uint8_t             bDeviceProtocol;
    uint8_t             bMaxPacketSize0;
    uint16_t            idVendor;
    uint16_t            idProduct;
    uint16_t            bcdDevice;
    uint8_t             iManufacturer;
    uint8_t             iProduct;
    uint8_t             iSerialNumber;
    uint8_t             bNumConfigurations;
} StandardDeviceDescriptor;

// StandardConfigurationDescriptor defined on p.9-37 
typedef struct __attribute__((packed)) {
    DescriptorHeader    hdr;
    uint16_t            wTotalLength;
    uint8_t             bNumInterfaces;
    uint8_t             bConfigurationValue;
    uint8_t             iConfiguration;
    uint8_t             bmAttributes;
    uint8_t             bMaxPower;
} StandardConfigurationDescripotr;

// StandardInterfaceDescripotr defined on p.9-40
typedef struct __attribute__((packed)) {
    DescriptorHeader    hdr;
    uint8_t             bInterfaceNumber;
    uint8_t             bAlternateSetting;
    uint8_t             bNumEndpoints;
    uint8_t             bInterfaceClass;
    uint8_t             bInterfaceSubClass;
    uint8_t             bInterfaceProtocol;
    uint8_t             iInterface;
} StandardInterfaceDescriptor;

// StandardEndpointDescriptor defined on p.9-41
typedef struct __attribute__((packed)) {
    DescriptorHeader    hdr;
    uint8_t             bEndpointAddress;
    uint8_t             bmAttributes;
    uint16_t            wMaxPacketSize;
    uint8_t             bInterval;
} StandardEndpointDescriptor;

// HIDDescriptor defined on p.22 of "Device Class Definition for HID"
typedef struct __attribute__((packed)) {
    uint8_t             bLength;
    uint8_t             bDescriptorType;
    uint16_t            bcdHID;
    uint8_t             bCountryCode;
    uint8_t             bNumDescriptors;
    DescriptorHeader    *classDescriptors;
} HIDDesctiptor;

void PrintDeviceDescriptor(int slotID);
void PrintConfigurationDescriptor(int slotID);

#include <device/device.h> // 相互参照によるエラーを防ぐため

#endif