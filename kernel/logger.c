#include <logger.h>

LogLevel GlogLevel = Info;

void SetLogLevel(LogLevel l) {
    GlogLevel = l;
}

int Log(LogLevel l, char *fmt, ...) {
    char buf[BUF_SIZE];
    int n;

    if(l > GlogLevel)
        return 0;
    
    va_list ap;
    va_start(ap, fmt);
    n = kvsnprintf(
        buf,
        BUF_SIZE,
        fmt,
        ap
    );
    va_end(ap);
    
    putString(buf);

    return n;
}