#include <kstring.h>

unsigned int kstrnlen(const char *s, unsigned int maxlen) {
    unsigned int i;

    for(i = 0; *s++ && maxlen--; i++);

    return i;
}

void *kmemset(void *s, int c, unsigned int n) {
    char *p = s;
    while(n--)
        *p++ = c;
    
    return s;
}

void *kmemcpy(void *dst, const void *src, unsigned int len) {
    char *p = dst;

    while(len--)
        *p++ = *(char*)src++;
    
    return dst;
}

