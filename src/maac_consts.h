/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_CONSTS_INCLUDE_GUARD
#define MAAC_CONSTS_INCLUDE_GUARD

#include "maac_stdint.h"

#define MAAC_WINDOW_SEQUENCE_ONLY_LONG   0x00
#define MAAC_WINDOW_SEQUENCE_LONG_START  0x01
#define MAAC_WINDOW_SEQUENCE_EIGHT_SHORT 0x02
#define MAAC_WINDOW_SEQUENCE_LONG_STOP   0x03

#define MAAX_MAX_WINDOW_GROUPS MAAC_U32_C(8)

/* 32 kHz has the highest swb value of 50 */
#define MAAC_MAX_SWB_OFFSET_LONG_WINDOW  MAAC_U32_C(51)

/* a few scale factor band tables have a max short swb of 14 */
#define MAAC_MAX_SWB_OFFSET_SHORT_WINDOW MAAC_U32_C(15)

#define MAAC_MAX_SECTIONS 120

#define MAAC_ZERO_HCB 0
#define MAAC_PAIR_LEN 2
#define MAAC_QUAD_LEN 4
#define MAAC_FIRST_PAIR_HCB 5
#define MAAC_ESC_HCB 11
#define MAAC_NOISE_HCB 13
#define MAAC_INTENSITY_HCB2 14
#define MAAC_INTENSITY_HCB 15
#define MAAC_ESC_FLAG 16

/* in the main profile, TNS_MAX_ORDER_1024 is 20 - but
   we currently only support the LC profile, where the
   max order is 12 */
#define MAAC_TNS_MAX_ORDER_1024 12
#define MAAC_TNS_MAX_ORDER_128 7

/* various order processing stuff includes the max order value */
#define MAAC_TNS_MAX_ORDER 12
#define MAAC_TNS_TOTAL_ORDER (MAAC_TNS_MAX_ORDER_1024+1)

#endif /* INCLUDE_GUARD */

