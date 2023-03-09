#ifndef __K_STDLIB_H__
#define __K_STDLIB_H__

#include <font.h>
#include <kbuiltin.h>
#include <kstring.h>
#include <console.h>
#include <stddef.h>
#include <stdint.h>

struct cookie {
	char *s;
	unsigned int n;
};

int kvsnprintf(char *restrict s, unsigned int n, const char *restrict fmt, kva_list ap);
int kprintf(const char *fmt, ...);

#endif 