#ifndef __K_STR_H__
#define __K_STR_H__

unsigned int kstrnlen(const char *s, unsigned int maxlen);
void *kmemset(void *s, int c, unsigned int n);
void *kmemcpy(void *dst, const void *src, unsigned int len);

#endif 