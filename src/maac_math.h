/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_MATH_INCLUDE_GUARD
#define MAAC_MATH_INCLUDE_GUARD

#include "maac_visibility.h"
#include "maac_float.h"
#include "maac_stdint.h"

/* returns 1.0 / sqrt(x) */
MAAC_PRIVATE
maac_flt maac_inv_sqrt(maac_flt x);

/* returns pow(2, x/4.0), used for scaling */
MAAC_PRIVATE
maac_flt maac_pow2_xdiv4(maac_s16 x);

/* returns the cube root of x, used when
   inverse quantizing */
MAAC_PRIVATE
maac_flt maac_cbrt(maac_u16 x);

#define MAAC_M_PI MAAC_FLT_C(3.14159265358979323846)

#endif /* INCLUDE_GUARD */

