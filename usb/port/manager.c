#include <port/manager.h>

// 全てのPortの状態を管理するテーブル(MaxPorts個のエントリを持つ)
// PortIDは1からMaxPortsまでの値を取る
static PortStateTable table;

UsbError InitPortManager(void) {
    // Portの状態を管理するテーブルを作成
    table = (PortStateTable) {
        .len    = cap->HCSPARAMS1.bits.MaxPorts,
        .state  =  (PortState *)AllocMem(
            sizeof(PortState) * cap->HCSPARAMS1.bits.MaxPorts,
            0,
            0
        )
    };

    if(!table.state)
        return ErrLowMemory;
    
    return ErrSuccess;
}

PORTSCBitmap GetPORTSC(int portID) {
    return  pr[portID - 1].PORTSC;
}

static void setPORTSC(int portID, PORTSCBitmap portsc) {
    pr[portID - 1].PORTSC = portsc;
}

void SetPORTSCFlags(int portID, uint32_t flags) {
    PORTSCBitmap portsc = GetPORTSC(portID);
    
    portsc.data &= FLAGMASK;
    portsc.data |= flags;

    setPORTSC(portID, portsc);
}

// clearPORTSCFlags shoud be used to clear RW1CS flags
// this function is alias for SetPORTSCFlags
void ClearPORTSCFlags(int portID, uint32_t flags) {
    SetPORTSCFlags(portID, flags);
}

static PortState getState(int portID) {
    return table.state[portID - 1];
}

static void setState(int portID, PortState s) {
    table.state[portID - 1] = s;
}

UsbError TransitionState(int portID, PortState to) {
    PortState from  = getState(portID);
    switch(to) {
        case PortStateEnabled:
            if(from != PortStateDisconnected && from != PortStateDisabled)
                return ErrInvalidPortStateTransition;
            break;
        case PortStateDisabled:
            if(from != PortStateDisconnected)
                return ErrInvalidPortStateTransition;
            break;
        default:
            return ErrInvalidPortStateTransition;
    }
    printk("port%#x from: %#x to: %#x\n", portID, from, to);
    setState(portID, to);

    return ErrSuccess;
}

UsbError ResetPort(int portID) {
    SetPORTSCFlags(portID, _PR);

    return ErrSuccess;
}