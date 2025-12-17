/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_FILL_ELEMENT_EXTRAS_INCLUDE_GUARD
#define MAAC_FILL_ELEMENT_EXTRAS_INCLUDE_GUARD

#include "fil.h"

MAAC_CDECLS_BEGIN

maac_const
MAAC_PUBLIC
size_t
maac_fil_size(void);

maac_const
MAAC_PUBLIC
size_t
maac_fil_alignof(void);

maac_pure
MAAC_PUBLIC
maac_fil*
maac_fil_align(void*);

maac_pure
MAAC_PUBLIC
maac_u32
maac_fil_extension_type(const maac_fil* f);

maac_const
MAAC_PUBLIC
const char*
maac_fil_extension_type_name(const maac_u32 t);

maac_const
MAAC_PUBLIC
size_t
maac_fil_extension_type_name_len(const maac_u32 t);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
