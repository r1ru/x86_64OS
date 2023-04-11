#ifndef __USB_ERROR_H__
#define __USB_ERROR_H__

#include <stdint.h>
#include <stdarg.h>

#include <kstdio.h>
#include <kstring.h>

// USBErrorCodeはエラーが何に関連するものなのかを表す。(大まかな分類)
typedef enum {
    ErrNil,
    ErrxHC,
    ErrPort,
    ErrMemory,
    ErrCommand,
    ErrEvent,
} USBErrorCode;

typedef struct {
    USBErrorCode code; 
    char *file;
    int line; 
    char *info;
} USBError;

extern USBError Nil;

#define NewErrorf(c, ...) _NewErrorf(c, __FILE__, __LINE__, __VA_ARGS__)
USBError _NewErrorf(USBErrorCode c, char *f,  int l, char *fmt, ...);
void PrintError(USBError err);

#endif