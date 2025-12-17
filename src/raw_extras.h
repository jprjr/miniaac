/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_RAW_EXTRAS_INCLUDE_GUARD
#define MAAC_RAW_EXTRAS_INCLUDE_GUARD

#include "raw.h"

MAAC_CDECLS_BEGIN

/* helper - returns the number of channels indicated by channel config,
only useful if you used maac_config with an AudioSpecificConfig */
maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_channels(const maac_raw* r);

maac_const
MAAC_PUBLIC
size_t
maac_raw_size(void);

maac_const
MAAC_PUBLIC
size_t
maac_raw_alignof(void);

maac_pure
MAAC_PUBLIC
maac_raw*
maac_raw_align(void*);

maac_const
MAAC_PUBLIC
const char*
maac_raw_data_block_id_name(const maac_u32 id);

maac_const
MAAC_PUBLIC
size_t
maac_raw_data_block_id_name_len(const maac_u32 id);

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_state(const maac_raw* r);

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_sf_index(const maac_raw* r);

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_ele_id(const maac_raw* r);

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_sample_rate(const maac_raw* r);

maac_pure
MAAC_PUBLIC
const maac_channel* maac_raw_out_channels(const maac_raw* r);

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_num_out_channels(const maac_raw* r);

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_channel_configuration(const maac_raw* r);

MAAC_PUBLIC
maac_fil*
maac_raw_fil(maac_raw* r);

MAAC_PUBLIC
maac_sce*
maac_raw_sce(maac_raw* r);

MAAC_PUBLIC
maac_cpe*
maac_raw_cpe(maac_raw* r);

MAAC_PUBLIC
void
maac_raw_set_out_channels(maac_raw* r, maac_channel* ch);

MAAC_PUBLIC
void
maac_raw_set_num_out_channels(maac_raw* r, maac_u32 n);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
