/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_SFG_NUM_WINDOW_GROUPS_INCLUDE_GUARD
#define MAAC_SFG_NUM_WINDOW_GROUPS_INCLUDE_GUARD

#include "maac_const.h"
#include "maac_stdint.h"
#include "maac_inline.h"
#include "maac_popcnt.h"

maac_const static maac_inline
maac_u32
maac_sfg_num_window_groups(maac_u8 scale_factor_grouping) {
    return 1 + (7 - maac_popcnt((maac_u32)scale_factor_grouping));
}

#endif /* INCLUDE_GUARD */
