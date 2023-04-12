#ifndef __K_STDLIB_H__
#define __K_STDLIB_H__

#include <font.h>
#include <kstring.h>
#include <console.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define BUF_SIZE 100

struct cookie {
	char *s;
	unsigned int n;
};

int kvsnprintf(char *restrict s, unsigned int n, const char *restrict fmt, va_list ap);
int printk(const char *fmt, ...);

#endif 