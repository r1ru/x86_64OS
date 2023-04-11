#include <usbError.h>

#define MAX_INFO_LEN 50
static char info[MAX_INFO_LEN];

USBError Nil = (USBError) {
    .code = ErrNil,
    .line = 0,
    .file = "",
    .info = ""
};

USBError _NewErrorf(USBErrorCode c, char *f,  int l, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    kvsnprintf(
        info, 
        MAX_INFO_LEN,
        fmt,
        ap
    );
    va_end(ap);

    return (USBError) {
        .code = c,
        .file = f,
        .line = l,
        .info = info
    };
}

void PrintError(USBError err) {
    if(err.code == ErrNil)
        return;

    printk(
        "%s:%d: error: %#x %s\n",
        err.file,
        err.line,
        err.code,
        err.info
    );
}