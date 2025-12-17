/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_BITREADER_EXTRAS_INCLUDE_GUARD
#define MAAC_BITREADER_EXTRAS_INCLUDE_GUARD

#include "bitreader.h"

MAAC_CDECLS_BEGIN

maac_const
MAAC_PUBLIC
size_t
maac_bitreader_size(void);

maac_const
MAAC_PUBLIC
size_t
maac_bitreader_alignof(void);

MAAC_PUBLIC
maac_bitreader*
maac_bitreader_align(void*);

/* returns how many bytes are left to be read by the bitreader */
maac_pure
MAAC_PUBLIC
maac_u32
maac_bitreader_avail(const maac_bitreader* br);

/* setter functions */

MAAC_PUBLIC
void
maac_bitreader_set_data(maac_bitreader* br, const maac_u8* data);

MAAC_PUBLIC
void
maac_bitreader_set_pos(maac_bitreader* br, maac_u32 pos);

MAAC_PUBLIC
void
maac_bitreader_set_len(maac_bitreader* br, maac_u32 len);

/* getter functions */

maac_pure
MAAC_PUBLIC
const maac_u8*
maac_bitreader_data(const maac_bitreader* br);

maac_pure
MAAC_PUBLIC
maac_u32
maac_bitreader_pos(const maac_bitreader* br);

maac_pure
MAAC_PUBLIC
maac_u32
maac_bitreader_len(const maac_bitreader* br);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
