/* SPDX-License-Identifier: 0BSD */
#include "maac_rand.h"

static maac_u32 maac_random_seed = MAAC_U32_C(0xbabab0ee);

MAAC_PUBLIC
void
maac_srand(maac_u32 val) {
    maac_random_seed = val;
}

MAAC_PRIVATE
maac_u32
maac_rand_seed(void) {
    return maac_random_seed;
}
