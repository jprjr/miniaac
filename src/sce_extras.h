/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_SINGLE_CHANNEL_ELEMENT_EXTRAS_INCLUDE_GUARD
#define MAAC_SINGLE_CHANNEL_ELEMENT_EXTRAS_INCLUDE_GUARD

#include "sce.h"

MAAC_CDECLS_BEGIN

maac_const
MAAC_PUBLIC
size_t
maac_sce_size(void);

maac_const
MAAC_PUBLIC
size_t
maac_sce_alignof(void);

maac_pure
MAAC_PUBLIC
maac_sce*
maac_sce_align(void*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_sce_state(const maac_sce*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_sce_element_instance_tag(const maac_sce*);

maac_const
MAAC_PUBLIC
const char*
maac_sce_state_name(maac_u32 state);

maac_const
MAAC_PUBLIC
size_t
maac_sce_state_name_len(maac_u32 state);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */

