#ifndef __MACROS_H__
#define __MACROS_H__

// useful macros
#define MERGE(a,b)  a##b
#define LABEL(a) MERGE(_Rsvd, a)
#define Rsvd LABEL(__LINE__)

#endif 