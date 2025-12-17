/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_FILTERBANK_INCLUDE_GUARD
#define MAAC_FILTERBANK_INCLUDE_GUARD

#include "maac_common.h"

struct maac_filterbank_params {
    maac_u8 window_sequence;
    maac_u8 window_shape;
    maac_u8 window_shape_prev;
};

typedef struct maac_filterbank_params maac_filterbank_params;

MAAC_PRIVATE
void
maac_filterbank(maac_flt* samples, maac_flt* overlap, const maac_filterbank_params* p);

#endif /* INCLUDE_GUARD */
