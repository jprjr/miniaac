/* SPDX-License-Identifier: 0BSD */
#include "scalefactor_bands.h"
#include "scalefactor_band_data.h"
#include "maac_consts.h"

maac_pure
MAAC_PRIVATE
maac_scalefactor_bands maac_scalefactor_bands_long(maac_u8 sf_index) {
    maac_scalefactor_bands s = { NULL, 0 };

    s.offsets = &maac_swb_offset_long_window[maac_swb_offset_long_window_index[sf_index]];
    s.len = maac_num_swb_long_window[sf_index];

    return s;
}

maac_pure
MAAC_PRIVATE
maac_scalefactor_bands maac_scalefactor_bands_short(maac_u8 sf_index) {
    maac_scalefactor_bands s = { NULL, 0 };

    s.offsets = &maac_swb_offset_short_window[maac_swb_offset_short_window_index[sf_index]];
    s.len = maac_num_swb_short_window[sf_index];

    return s;
}

maac_pure
MAAC_PRIVATE
maac_scalefactor_bands maac_scalefactor_bandsf(maac_u8 window_sequence, maac_u8 sf_index) {
    return window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ?
      maac_scalefactor_bands_short(sf_index)
      :
      maac_scalefactor_bands_long(sf_index);
}
