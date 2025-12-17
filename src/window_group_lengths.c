/* SPDX-License-Identifier: 0BSD */
#include "window_group_lengths.h"
#include "maac_consts.h"

maac_const
MAAC_PRIVATE
maac_u32
maac_window_group_lengths(maac_u8 window_sequence, maac_u8 scale_factor_grouping) {
    maac_u32 lengths = 0;
    maac_u8 shift = 0;
    maac_u8 len = 1;
    maac_u8 b = 7;

    if(window_sequence != MAAC_WINDOW_SEQUENCE_EIGHT_SHORT) return 1;

    while(b--) {
        if( (scale_factor_grouping >> b) & 0x01) {
            len++;
        } else {
            lengths |= (len & 0x0f) << shift;
            shift += 4;
            len = 1;
        }
    }
    lengths |= (len << shift);
    return lengths;
}

