/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_FREQUENCY_INCLUDE_GUARD
#define MAAC_FREQUENCY_INCLUDE_GUARD

#include "maac_stdint.h"
#include "maac_visibility.h"
#include "maac_const.h"

maac_const
MAAC_PUBLIC
maac_u32
maac_sampling_frequency_index(maac_u32 sample_rate);

maac_const
MAAC_PUBLIC
maac_u32
maac_sampling_frequency(maac_u32 sample_frequency_index);

#endif /* INCLUDE_GUARD */
