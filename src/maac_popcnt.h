/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_POPCNT_INCLUDE_GUARD
#define MAAC_POPCNT_INCLUDE_GUARD

#if defined(__GNUC__) && __GNUC__ >= 4
#define maac_popcnt(x) __builtin_popcount(x)
#elif defined(_MSC_VER)
#include <intrin.h>
#define maac_popcnt(x) __popcnt(x)
#else

#include "maac_stdint.h"

static maac_inline
maac_u32 maac_popcnt(maac_u32 val) {
    maac_u32 c = 0;
    while(val) {
        val &= val - 1;
        ++c;
    }
    return c;
}

#endif

#endif /* INCLUDE_GUARD */

