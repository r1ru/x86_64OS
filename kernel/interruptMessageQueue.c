#include <interruptMessageQueue.h>

static Queue q = {
    .writeIdx   = 0,
    .readIdx    = 0,
    .len        = 0 
};

bool isEmpty(void) {
    return q.len == 0;
}

// if queue is full, msg is ignored 
void pushMsg(InterruptMessage msg) {
    if(q.len == QSIZE) return;
    
    q.data[q.writeIdx] = msg;
    q.len++;
    q.writeIdx = (q.writeIdx + 1) % QSIZE;
}

// Check if there is an entry in queue before calling popMsg
// isEmpty function can be used for this purpose.
InterruptMessage popMsg(void) {
    InterruptMessage msg = q.data[q.readIdx];
    asm volatile("cli"); // clear IF 
    q.len--;
    asm volatile("sti"); // set IF
    q.readIdx = (q.readIdx + 1) % QSIZE;
    return msg;
}