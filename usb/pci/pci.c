#include <pci/pci.h>

#define shl(addr, n) (addr) << (n)

#define ConfigAddres 0x0cf8
#define ConfigData 0x0cfc

static int NumDevice;

Device xhcDev;

// CONFIG_ADDRESS(0x0cf8):
//  |   31 | 30:24 | 23:16 | 15:11 | 10:8 | 7:0 |
//  | ---- | ---- | ---- | ---- | ---- | ---- |
//  | Enable bit | reserved | bus number(0-0xff) | dev number(0-0x0f) | func number(0-0x7) | reg offset(4byte単位) |
static uint32_t makeAddress(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg_offset) {
    return shl(1, 31) | shl(bus, 16) | shl(dev, 11) | shl(func, 8) | (reg_offset & 0xfcu);
}

// writeData write data to specified addres
static void writeData(uint32_t address, uint32_t data) {
    ioOut32(ConfigAddres, address);
    ioOut32(ConfigData, data);
}

// readData read data from specified address
static uint32_t readData(uint32_t address) {
    ioOut32(ConfigAddres, address);
    return ioIn32(ConfigData);
}

static uint16_t readVendorId(uint8_t bus, uint8_t dev, uint8_t func) {
    return readData(makeAddress(bus, dev, func, 0x00)) & 0xffffu; // enable lower 16 bits
}

static uint32_t readClassId(uint8_t bus, uint8_t dev, uint8_t func) {
    return readData(makeAddress(bus, dev, func, 0x08));
}

// header_type:
//  | 31:24 | 23:16 | 15:8 | 
//  | ---- | ---- | ---- |
//  | BaseClass | SubClass | ProgrammingInterface |
static uint8_t readHeaderType(uint8_t bus, uint8_t dev, uint8_t func) {
    return (readData(makeAddress(bus, dev, func, 0x0c)) >> 16) & 0xffu;
}

//  BAR:
//  | 31:4 | 3 | 2:1 | 0 |
//  | ---- | ---- | ---- | ---- |
//  | base address | prefetch enable flag | type | メモリ空間インジケータ
//  64bitの場合、BAR0には下位32bit、BAR1には上位32bitが入る。BAR0は下位4bitをマスクして使う
uint64_t readBAR(uint8_t bus, uint8_t dev, uint8_t func) {
    uint32_t lower = readData(makeAddress(bus, dev, func, 0x10)) & 0xfffffff0u; // mask lower 4bits
    uint64_t upper = readData(makeAddress(bus, dev, func, 0x14));
    return (upper << 32) | lower;
}

// @0x18:
//  | 31:24 | 23:16 | 15:8 | 7:0 | 
//  | ---- | ---- | ---- | ---- | 
//  | - | - | Secondary Bus Number | Primary Bus Number | 
static uint32_t readBusNumber(uint8_t bus, uint8_t dev, uint8_t func) {
    return readData(makeAddress(bus, dev, func, 0x18));
}

// header_typeのbit8が立っている場合はmultiFunctionDevice
static bool isSingleFunctionDevice(uint8_t header_type) {
    return ((header_type & (1U << 8)) == 0);
}

static void scanBus(uint8_t bus); // prototype

static void scanFunc(uint8_t bus, uint8_t dev, uint8_t func) {
    // vendorId = 0xffffの場合は無効なfunction
    uint16_t vendorId = readVendorId(bus, dev, func);
    if(vendorId == 0xffffu)
        return;
    
    uint32_t classId = readClassId(bus, dev, func);
    uint8_t base = (classId >> 24) & 0xffu;
    uint8_t sub = (classId >> 16) & 0xffu;
    uint8_t interface = (classId >> 8) & 0xffu;

    // PCI-to-PCI bridgeの場合
    if(base == 0x06u && sub == 0x04u) {
        uint32_t secondaryBus = (readBusNumber(bus, dev, func) >> 8) & 0xffu;
        Log(
            Debug,
            "[*] found PCI-to-PCI bridge: secondaryBus: %#x\n", 
            secondaryBus
        );
        scanBus(secondaryBus);
    } else {
        Log(
            Debug,
            "[*] %d.%d.%d base: %#x, sub: %#x, interface: %#x vendorID: %#x\n",
            bus, dev, func,
            base,
            sub,
            interface,
            vendorId
        );
        // xHCだった場合
        if(base == 0x0c && sub == 0x03 && interface == 0x30) {
            xhcDev.bus = bus;
            xhcDev.dev = dev;
            xhcDev.func = func;
        }
        NumDevice++;
    }
}

static void scanDev(uint8_t bus, uint8_t dev) {
    // vendorId = 0xffffの場合は無効なデバイス
    uint16_t vendorId = readVendorId(bus, dev, 0);
    if(vendorId == 0xffffu)
        return;
    
    uint8_t headerType = readHeaderType(bus, dev, 0);
    if(isSingleFunctionDevice(headerType)) {
        scanFunc(bus, dev, 0);
    }
    else {
        // 1つのデバイスは最大8個のfunctionを持つ
        for(uint8_t func = 0; func < 8; func++) {
            scanFunc(bus, dev, func);
        }
    }
}

// 一つのバスにつき最大32個のデバイスが接続される。
static void scanBus(uint8_t bus) {
    for(uint8_t dev = 0; dev < 32; dev++) {
        scanDev(bus, dev);
    }
}

int scanAllBus(void) {
    uint8_t headerType = readHeaderType(0, 0, 0); // Host bridgeのheaderを読む
    if(isSingleFunctionDevice(headerType)){
        scanBus(0);
    }else{
        // multiFunctionDeviceの場合はfunction nのHost Bridgeがbus nを担当する。 
        for(int func = 0; func < 8; func++) {
            if(readVendorId(0, 0, func) == 0xffffu)
                continue;
            scanBus(func);
        }
    }
    return NumDevice;
}

// configureMSI expects the MSI Capability will always be present
void configureMSI(Device dev, MessageAddressBitmap msgAddr, MessageDataBitmap msgData) {
    uint32_t base   = makeAddress(dev.bus, dev.dev, dev.func, 0);
    uint8_t  ptr    = readData(base + 0x34) & 0xffu;
    uint32_t addr   = base + ptr;
    
    HeaderBitmap hdr  = (HeaderBitmap)readData(addr);

    while(hdr.bits.CapId != 0x5) {
        addr = base + hdr.bits.NxtPtr;
        hdr = (HeaderBitmap)readData(addr);
    }

    Log(
        Debug,
        "[*] MSI Capability: CapId: %#x NxtPtr: %#x Addr64: %#x PreVectorMasking: %#x\n",
        hdr.bits.CapId,
        hdr.bits.NxtPtr,
        hdr.bits.Addr64Capable,
        hdr.bits.PreVectorMaskingCapable
    );

    // headerを設定
    hdr.bits.MSIEnable = 1;
    hdr.bits.MultipleMessageEnable = 0; // Primary Interrupterのみ
    writeData(addr, hdr.data);
    
    // Message Address, MessageDataを設定
    writeData(addr + 4, msgAddr.data);
    writeData(addr + 12, msgData.data);
}