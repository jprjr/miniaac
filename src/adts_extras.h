/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_ADTS_EXTRAS_INCLUDE_GUARD
#define MAAC_ADTS_EXTRAS_INCLUDE_GUARD

#include "adts.h"

MAAC_CDECLS_BEGIN

/* these are functions to make runtime/ffi querying easier,
when an implementation doesn't have the full adts struct
definition available */

maac_const
MAAC_PUBLIC
size_t
maac_adts_size(void);

maac_const
MAAC_PUBLIC
size_t
maac_adts_alignof(void);

maac_pure
MAAC_PUBLIC
maac_adts*
maac_adts_align(void*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_tolerance(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_mpeg_version(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_layer(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_protection_absent(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_profile(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_frequency_index(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_sample_rate(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_channel_configuration(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_channels(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_original_copy(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_home(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_copyright_id_bit(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_copyright_id_start(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_frame_length(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_buffer_fullness(const maac_adts* a);

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_raw_data_blocks(const maac_adts* a);

/* get the underlying maac_raw object used to decode */
maac_pure
MAAC_PUBLIC
maac_raw*
maac_adts_raw(maac_adts* a);

/* sets a tolerance value for how many bytes to read during the first sync,
   so for example if you start reading from the middle of a file, the maac_adts
   object won't immediately error out - it will try reading bytes for a while
   until it sees a syncword.

   If unset, default is 0 - meaning the data must immediately start on an ADTS
   syncword */

MAAC_PUBLIC
void
maac_adts_set_tolerance(maac_adts* a, maac_u32 tolerance);

MAAC_PUBLIC
void
maac_adts_set_out_channels(maac_adts* a, maac_channel* ch);

MAAC_PUBLIC
void
maac_adts_set_num_out_channels(maac_adts* a, maac_u32 num);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
