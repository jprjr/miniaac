/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_SECTION_IDX_INCLUDE_GUARD
#define MAAC_SECTION_IDX_INCLUDE_GUARd

#include "maac_consts.h"

/* max is 120 */
#define maac_section_idx(g,i) ( ((MAAC_MAX_SWB_OFFSET_SHORT_WINDOW) * (g)) + (i))

#endif /* INCLUDE_GUARD */
