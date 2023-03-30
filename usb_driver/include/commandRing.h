#ifndef __COMMAND_RING_H__
#define __COMMAND_RING_H__

#include <stdint.h>
#include <stdalign.h>

#include <trb.h>
#include <registers.h>

typedef enum {
    CommandRequested,
    InvalidCommand
} CommandRingError;

#define CRSIZE      0x10
typedef struct {
    TRB buf[CRSIZE];
    int PCS; // Producer Cycle State
    int writeIdx;
} CommandRing;

void initCommandRing(void);
CommandRingError pushCommand(TRBType ty);

#endif 