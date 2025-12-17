/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_PERCEPTUAL_NOISE_SUB_INCLUDE_GUARD
#define MAAC_PERCEPTUAL_NOISE_SUB_INCLUDE_GUARD

#include "maac_common.h"
#include "maac_float.h"
#include "ics.h"

struct maac_pns_params {
    maac_flt* spectra;
    const maac_ics* ics;
    maac_u8 sf_index;
    maac_u32* rand_state;
};
typedef struct maac_pns_params maac_pns_params;

MAAC_PRIVATE
void
maac_pns_process(const maac_pns_params* para);

#endif /* INCLUDE_GUARD */

