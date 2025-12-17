/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_RAND_INCLUDE_GUARD
#define MAAC_RAND_INCLUDE_GUARD

#include "maac_common.h"
#include "maac_inline.h"

MAAC_CDECLS_BEGIN

/* set a default seed for the random number generator,
which will be inherited in structs */
MAAC_PUBLIC
void
maac_srand(maac_u32 val);

MAAC_CDECLS_END

/* get the default srand seed */

MAAC_PRIVATE
maac_u32
maac_rand_seed(void);

/* classic ranqd1 - quick and dirty rand from
   Numerical Recipes */
static maac_inline maac_u32
maac_rand(maac_u32* s) {
#if 0
    return *s = (*s) * (MAAC_U32_C(1664525) + MAAC_U32_C(1013904223));
#endif
    return *s = (*s) * MAAC_U32_C(1664525) + MAAC_U32_C(1013904223);
}

#endif /* INCLUDE_GUARD */
