#ifndef __INTERRUPT_MESSAGE_QUEUE_H__
#define __INTERRUPT_MESSAGE_QUEUE_H__

#include <stdbool.h>

typedef enum {
    InterruptXHCI
} InterruptMessageType;

typedef struct {
    InterruptMessageType ty;
} InterruptMessage;

#define QSIZE   0x10
typedef struct {
    InterruptMessage data[QSIZE];
    int writeIdx;
    int readIdx;
    int len;
} Queue;

bool isEmpty(void);
void pushMsg(InterruptMessage msg);
InterruptMessage popMsg(void);

#endif 