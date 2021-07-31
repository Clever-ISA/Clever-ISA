#ifndef __STDDEF_H_2021_07_30_20_10_54
#define __STDDEF_H_2021_07_30_20_10_54

typedef unsigned long size_t;
typedef long ptrdiff_t;

#ifdef __SIZEOF_INT128__
typedef struct{ __uint128 __inner;} max_align_t;
#else
typedef struct{ unsigned long long __inner;} max_align_t;
#endif

#if __has_include_next(<stddef.h>)
#include_next <stddef.h>
#endif


#endif /* defined(__STDDEF_H_2021_07_30_20_10_54)*/
