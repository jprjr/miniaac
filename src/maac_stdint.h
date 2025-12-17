/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_STDINT_INCLUDE_GUARD
#define MAAC_STDINT_INCLUDE_GUARD

#if defined(__cplusplus) && __cplusplus >= 201103L

#include <cstdint>

typedef uint8_t   maac_u8;
typedef  int8_t   maac_s8;
typedef uint16_t  maac_u16;
typedef  int16_t  maac_s16;
typedef uint32_t  maac_u32;
typedef  int32_t  maac_s32;

#define MAAC_U8_C(x)   UINT8_C(x)
#define MAAC_U16_C(x) UINT16_C(x)
#define MAAC_U32_C(x) UINT32_C(x)

#define MAAC_S8_C(x)   INT8_C(x)
#define MAAC_S16_C(x) INT16_C(x)
#define MAAC_S32_C(x) INT32_C(x)

#define MAAC_S8_MAX INT8_MAX
#define MAAC_S8_MIN INT8_MIN

#define MAAC_S16_MAX INT16_MAX
#define MAAC_S16_MIN INT16_MIN

#define MAAC_S32_MAX INT32_MAX
#define MAAC_S32_MIN INT32_MIN

#define MAAC_U8_MAX  UINT8_MAX
#define MAAC_U16_MAX UINT16_MAX
#define MAAC_U32_MAX UINT32_MAX

#elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (defined(__GNUC__) && (__GNUC__ > 3 || defined(_STDINT_H_)))

#include <stdint.h>

typedef uint8_t   maac_u8;
typedef  int8_t   maac_s8;
typedef uint16_t  maac_u16;
typedef  int16_t  maac_s16;
typedef uint32_t  maac_u32;
typedef  int32_t  maac_s32;

#define MAAC_U8_C(x)   UINT8_C(x)
#define MAAC_U16_C(x) UINT16_C(x)
#define MAAC_U32_C(x) UINT32_C(x)

#define MAAC_S8_C(x)   INT8_C(x)
#define MAAC_S16_C(x) INT16_C(x)
#define MAAC_S32_C(x) INT32_C(x)

#define MAAC_S8_MAX INT8_MAX
#define MAAC_S8_MIN INT8_MIN

#define MAAC_S16_MAX INT16_MAX
#define MAAC_S16_MIN INT16_MIN

#define MAAC_S32_MAX INT32_MAX
#define MAAC_S32_MIN INT32_MIN

#define MAAC_U8_MAX  UINT8_MAX
#define MAAC_U16_MAX UINT16_MAX
#define MAAC_U32_MAX UINT32_MAX

#else /* pre C99/C++11/GCC support */

#if defined(__cplusplus)
#include <climits>
#else
#include <limits.h>
#endif

#define MAAC_U8_MAX  0xFF
#define MAAC_S8_MAX  0x7F
#define MAAC_U16_MAX 0xFFFF
#define MAAC_S16_MAX 0x7FFF
#define MAAC_U32_MAX 0xFFFFFFFFUL
#define MAAC_S32_MAX 0x7FFFFFFFL

#if (MAAC_U8_MAX == UCHAR_MAX)
typedef unsigned char maac_u8;
#define MAAC_U8_C(x) ((maac_u8)x)
#else
#error "Unable to determine suitable u8 type"
#endif

#if (MAAC_S8_MAX == SCHAR_MAX)
typedef signed char maac_s8;
#define MAAC_S8_C(x) ((maac_s8)x)
#define MAAC_S8_MIN SCHAR_MIN
#else
#error "Unable to determine suitable s8 type"
#endif


#if (MAAC_U16_MAX == USHRT_MAX)
typedef unsigned short maac_u16;
#define MAAC_U16_C(x) ((maac_u16)x)
#else
#error "Unable to determine suitable u16 type"
#endif

#if (MAAC_S16_MAX == SHRT_MAX)
typedef signed short maac_s16;
#define MAAC_S16_C(x) ((maac_s16)x)
#define MAAC_S16_MIN SHRT_MIN
#else
#error "Unable to determine suitable s16 type"
#endif


#if (MAAC_U32_MAX == UINT_MAX)
typedef unsigned int maac_u32;
#define MAAC_U32_C(x) (x ## U)
#elif (MAAC_U32_MAX == ULONG_MAX)
typedef unsigned long maac_u32;
#define MAAC_U32_C(x) (x ## UL)
#else
#error "Unable to determine suitable u32 type"
#endif

#if (MAAC_S32_MAX == INT_MAX)
typedef signed int maac_s32;
#define MAAC_S32_C(x) (x)
#define MAAC_S32_MIN SHRT_MIN
#elif (MAAC_S32_MAX == LONG_MAX)
typedef signed long maac_s32;
#define MAAC_S32_C(x) (x ## L)
#define MAAC_S32_MIN LONG_MIN
#else
#error "Unable to determine suitable s32 type"
#endif

#endif /* pre-C99/C++11 support */

#endif /* INCLUDE_GUARD */
