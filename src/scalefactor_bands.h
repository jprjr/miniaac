/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_SCALEFACTOR_WINDOW_BANDS_INCLUDE_GUARD
#define MAAC_SCALEFACTOR_WINDOW_BANDS_INCLUDE_GUARD

#include "maac_common.h"

struct maac_scalefactor_bands {
    const maac_u16* offsets;
    maac_u16 len;
};

typedef struct maac_scalefactor_bands maac_scalefactor_bands;

maac_pure
MAAC_PRIVATE
maac_scalefactor_bands maac_scalefactor_bands_long(maac_u8 sf_index);

maac_pure
MAAC_PRIVATE
maac_scalefactor_bands maac_scalefactor_bands_short(maac_u8 sf_index);

maac_pure
MAAC_PRIVATE
maac_scalefactor_bands maac_scalefactor_bandsf(maac_u8 window_sequence, maac_u8 sf_index);

#endif /* INCLUDE_GUARD */

