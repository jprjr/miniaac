/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_ICS_INFO_EXTRAS_INCLUDE_GUARD
#define MAAC_ICS_INFO_EXTRAS_INCLUDE_GUARD

#include "ics_info.h"

MAAC_CDECLS_BEGIN

maac_const
MAAC_PUBLIC
const char*
maac_ics_info_state_name(const maac_u32 state);

maac_const
MAAC_PUBLIC
size_t
maac_ics_info_state_name_len(const maac_u32 state);

maac_const
MAAC_PUBLIC
size_t
maac_ics_info_size(void);

maac_const
MAAC_PUBLIC
size_t
maac_ics_info_alignof(void);

maac_pure
MAAC_PUBLIC
maac_ics_info*
maac_ics_info_align(void*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_state(const maac_ics_info* ics_info);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_window_sequence(const maac_ics_info* ics_info);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_window_shape(const maac_ics_info* ics_info);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_max_sfb(const maac_ics_info* ics_info);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_scale_factor_grouping(const maac_ics_info* ics_info);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_num_window_groups(const maac_ics_info* ics_info);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_predictor_data_present(const maac_ics_info* ics_info);

#if MAAC_ENABLE_MAINPROFILE
maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_predictor_reset(const maac_ics_info* ics_info);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_predictor_reset_group_number(const maac_ics_info* ics_info);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_prediction_used(const maac_ics_info* ics_info, maac_u32 sfb);
#endif

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
