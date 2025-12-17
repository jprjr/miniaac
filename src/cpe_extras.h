/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_CHANNEL_PAIR_ELEMENT_EXTRAS_INCLUDE_GUARD
#define MAAC_CHANNEL_PAIR_ELEMENT_EXTRAS_INCLUDE_GUARD

#include "cpe.h"

MAAC_CDECLS_BEGIN

maac_const
MAAC_PUBLIC
size_t
maac_cpe_size(void);

maac_const
MAAC_PUBLIC
size_t
maac_cpe_alignof(void);

maac_pure
MAAC_PUBLIC
maac_cpe*
maac_cpe_align(void*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_cpe_state(const maac_cpe*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_cpe_element_instance_tag(const maac_cpe*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_cpe_common_window(const maac_cpe*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_cpe_ms_mask_present(const maac_cpe*);

maac_const
MAAC_PUBLIC
const char*
maac_cpe_state_name(maac_u32 state);

maac_const
MAAC_PUBLIC
size_t
maac_cpe_state_name_len(maac_u32 state);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */


