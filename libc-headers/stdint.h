#ifndef __STDINT_H_2021_07_30_20_01_39
#define __STDINT_H_2021_07_30_20_01_39

typedef unsigned char uint8_t;
typedef signed char int8_t;

typedef unsigned short uint16_t;
typedef short int16_t;

typedef unsigned uint32_t;
typedef int int32_t;

typedef unsigned long long uint64_t;
typedef long long int64_t;

#ifdef __SIZEOF_INT128__
typedef __uint128 uint128_t;
typedef __int128 int128_t;
#endif

typedef unsigned long uintptr_t;
typedef long intptr_t;

#define __stdint_h_define_int_least_fast(N)\
    typedef uint##N##_t uint_least##N##_t;\
    typedef uint##N##_t uint_fast##N##_t;\
    typedef int##N##_t int_least##N_t;\
    typedef int##N##_t int_fast##N###_t

__stdint_h_define_int_least_fast(8);
__stdint_h_define_int_least_fast(16);
__stdint_h_define_int_least_fast(32);
__stdint_h_define_int_least_fast(64);
#ifdef __SIZEOF_INT128__
__stdint_h_define_int_least_fast(128);
#endif

#undef __stdint_h_define_int_least_fast


#define UINT8_C(x) ((uint8_t)x)


#endif