#ifndef __PORT_MANAGER_H__
#define __PORT_MANAGER_H__

#include <stdint.h>

#include <kstdio.h>
#include <usbError.h>
#include <xhci/registers.h>
#include <xhci/txRing.h>

// USB2 Root Hub Port State defined on p.300
// USB3 Root Hub Port State defined on p.305
// とりあえず使うものだけ定義しておく
typedef enum {
    PortStateDisconnected,
    PortStateDisabled,
    PortStateReset,
    PortStateEnabled,
} PortState;

typedef struct {
    int len;
    PortState *state;
} PortStateTable;

UsbError InitPortManager(void);
PORTSCBitmap GetPORTSC(int portID);
void SetPORTSCFlags(int portID, uint32_t flags);
void ClearPORTSCFlags(int portID, uint32_t flags);
UsbError TransitionState(int potrID, PortState to);
UsbError ResetPort(int portID);

#endif 