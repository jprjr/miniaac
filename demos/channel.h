/* SPDX-License-Identifier: 0BSD */

#include "maac.h"

/* given a channel layout and channel number,
return the equivalent channel number for a WAVE file */

maac_u32 channel_map(maac_u8 ch_layout, maac_u32 ch_number);

/* return a dwChannelMask for a given layout */
maac_u32 channel_mask(maac_u8 ch_layout);
