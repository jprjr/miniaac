/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_INDIVIDUAL_CHANNEL_STREAM_EXTRAS_INCLUDE_GUARD
#define MAAC_INDIVIDUAL_CHANNEL_STREAM_EXTRAS_INCLUDE_GUARD

#include "ics.h"

MAAC_CDECLS_BEGIN

maac_const
MAAC_PUBLIC
size_t
maac_ics_size(void);

maac_const
MAAC_PUBLIC
size_t
maac_ics_alignof(void);

maac_pure
MAAC_PUBLIC
maac_ics*
maac_ics_align(void*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_state(const maac_ics* ics);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_global_gain(const maac_ics* ics);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_pulse_data_present(const maac_ics* ics);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_tns_data_present(const maac_ics* ics);

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_gain_control_data_present(const maac_ics* ics);

maac_const
MAAC_PUBLIC
const char*
maac_ics_state_name(const maac_u32 state);

maac_const
MAAC_PUBLIC
size_t
maac_ics_state_name_len(const maac_u32 state);


MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
