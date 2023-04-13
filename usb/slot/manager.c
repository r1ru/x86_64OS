#include <slot/manager.h>

// 全てのSLotの状態を管理するテーブル(MaxSlotsEn個のエントリを持つ)
// SlotIDは1からMaxSlotsEnまでの値を取る
static SlotStateTable table;

USBError InitSlotManager(void) {
    // Slotの状態を管理するテーブルを作成
    table = (SlotStateTable) {
        .len = op->CONFIG.bits.MaxSlotsEn,
        .state =  (SlotState *)AllocMem(sizeof(SlotState) * op->CONFIG.bits.MaxSlotsEn,
            0,
            0
        )
    };

    if(!table.state)
        return NewErrorf(ErrMemory, "could not allocate slot table");
    
    return Nil; 
}

SlotState GetSlotState(int slotID) {
    return table.state[slotID - 1];
}

static void setState(int slotID, SlotState s) {
    table.state[slotID - 1] = s;
}

USBError TransitionSlotState(int slotID, SlotState to) {
    SlotState from  = GetSlotState(slotID);
    switch(to) {
        case SlotStateEnabled:
            if(from != SlotStateDisabled)
                return NewErrorf(ErrSlot, "invalid slot state transition");
            break;
        case SlotStateAddressed:
            if(from != SlotStateEnabled)
                return NewErrorf(ErrSlot, "invalid slot state transition");
            break;
        default:
            return NewErrorf(ErrSlot, "invalid slot state transition");
    }
    setState(slotID, to);

    Log(
        Info,
        "[+] slot%#x: state changed from %#x to %#x\n",
        slotID,
        from,
        to
    );

    return Nil; 
}
