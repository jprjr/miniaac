/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_IMDCT_INCLUDE_GUARD
#define MAAC_IMDCT_INCLUDE_GUARD

#include "maac_common.h"
#include "maac_float.h"

/* len is total window size, ie - if we have 1024
   coefficients, we'll have 2048 samples get output
   - so 2048 is the value for len */
MAAC_PRIVATE
void
maac_imdct(maac_flt* out, maac_u16 len);

#endif /* INCLUDE_GUARD */

