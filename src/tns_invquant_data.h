/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_TNS_INVQUANT_DATA_INCLUDE_GUARD
#define MAAC_TNS_INVQUANT_DATA_INCLUDE_GUARD

#include "maac_float.h"

/* coef_compress = 0, coef_len = 3 */
static const maac_flt MAAC_TNS_INVQUANT_0_3[] = {
    /* value = 0 (0) */ MAAC_FLT_C(0.0000000000000000),
    /* value = 1 (1) */ MAAC_FLT_C(0.43388373911755806),
    /* value = 2 (2) */ MAAC_FLT_C(0.78183148246802969),
    /* value = 3 (3) */ MAAC_FLT_C(0.97492791218182351),
    /* value = 4 (-4) */ MAAC_FLT_C(-0.98480775301220802),
    /* value = 5 (-3) */ MAAC_FLT_C(-0.86602540378443860),
    /* value = 6 (-2) */ MAAC_FLT_C(-0.64278760968653925),
    /* value = 7 (-1) */ MAAC_FLT_C(-0.34202014332566871)
};

/* coef_compress = 0, coef_len = 4 */
static const maac_flt MAAC_TNS_INVQUANT_0_4[] = {
    /* value = 0 (0) */ MAAC_FLT_C(0.0000000000000000),
    /* value = 1 (1) */ MAAC_FLT_C(0.20791169081775931),
    /* value = 2 (2) */ MAAC_FLT_C(0.40673664307580015),
    /* value = 3 (3) */ MAAC_FLT_C(0.58778525229247314),
    /* value = 4 (4) */ MAAC_FLT_C(0.74314482547739413),
    /* value = 5 (5) */ MAAC_FLT_C(0.86602540378443860),
    /* value = 6 (6) */ MAAC_FLT_C(0.95105651629515353),
    /* value = 7 (7) */ MAAC_FLT_C(0.99452189536827329),
    /* value = 8 (-8) */ MAAC_FLT_C(-0.99573417629503447),
    /* value = 9 (-7) */ MAAC_FLT_C(-0.96182564317281904),
    /* value = 10 (-6) */ MAAC_FLT_C(-0.89516329135506234),
    /* value = 11 (-5) */ MAAC_FLT_C(-0.79801722728023949),
    /* value = 12 (-4) */ MAAC_FLT_C(-0.67369564364655721),
    /* value = 13 (-3) */ MAAC_FLT_C(-0.52643216287735572),
    /* value = 14 (-2) */ MAAC_FLT_C(-0.36124166618715292),
    /* value = 15 (-1) */ MAAC_FLT_C(-0.18374951781657034)
};

/* coef_compress = 1, coef_len = 3 */
static const maac_flt MAAC_TNS_INVQUANT_1_3[] = {
    /* value = 0 (0) */ MAAC_FLT_C(0.0000000000000000),
    /* value = 1 (1) */ MAAC_FLT_C(0.43388373911755806),
    /* value = 2 (-2) */ MAAC_FLT_C(-0.64278760968653925),
    /* value = 3 (-1) */ MAAC_FLT_C(-0.34202014332566871)
};

/* coef_compress = 1, coef_len = 4 */
static const maac_flt MAAC_TNS_INVQUANT_1_4[] = {
    /* value = 0 (0) */ MAAC_FLT_C(0.0000000000000000),
    /* value = 1 (1) */ MAAC_FLT_C(0.20791169081775931),
    /* value = 2 (2) */ MAAC_FLT_C(0.40673664307580015),
    /* value = 3 (3) */ MAAC_FLT_C(0.58778525229247314),
    /* value = 4 (-4) */ MAAC_FLT_C(-0.67369564364655721),
    /* value = 5 (-3) */ MAAC_FLT_C(-0.52643216287735572),
    /* value = 6 (-2) */ MAAC_FLT_C(-0.36124166618715292),
    /* value = 7 (-1) */ MAAC_FLT_C(-0.18374951781657034)
};

static const maac_flt* const MAAC_TNS_INVQUANT[] = {
    MAAC_TNS_INVQUANT_0_3, MAAC_TNS_INVQUANT_0_4, MAAC_TNS_INVQUANT_1_3, MAAC_TNS_INVQUANT_1_4
};

#endif /* INCLUDE_GUARD */
