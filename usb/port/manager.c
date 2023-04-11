#include <port/manager.h>

// 全てのPortの状態を管理するテーブル(MaxPorts個のエントリを持つ)
// PortIDは1からMaxPortsまでの値を取る
static PortStateTable table;

USBError InitPortManager(void) {
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
        return NewErrorf(ErrMemory, "could not allocate port table");
    
    return Nil; 
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

USBError TransitionState(int portID, PortState to) {
    PortState from  = getState(portID);
    switch(to) {
        case PortStateEnabled:
            if(from != PortStateDisconnected && from != PortStateDisabled)
                return NewErrorf(ErrPort, "invalid port state transition");
            break;
        case PortStateDisabled:
            if(from != PortStateDisconnected)
                return NewErrorf(ErrPort, "invalid port state transition");
            break;
        default:
            return NewErrorf(ErrPort, "invalid port state transition");
    }
    setState(portID, to);

    return Nil; 
}

USBError ResetPort(int portID) {
    SetPORTSCFlags(portID, _PR);
    return Nil;
}