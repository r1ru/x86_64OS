#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdarg.h>

#include <console.h>
#include <kstdio.h>

typedef enum {
    Error,
    Info,
    Warning,
    Debug,
} LogLevel;

extern LogLevel GlogLevel;

void SetLogLevel(LogLevel l);
int Log(LogLevel l, char *fmt, ...);

#endif 