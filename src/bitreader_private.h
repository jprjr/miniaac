/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_BITREADER_PRIVATE_INCLUDE_GUARD
#define MAAC_BITREADER_PRIVATE_INCLUDE_GUARD

#include "bitreader.h"

MAAC_PRIVATE
MAAC_RESULT
maac_bitreader_fill(maac_bitreader* br, maac_u8 bits);

MAAC_PRIVATE
maac_u32
maac_bitreader_peek(const maac_bitreader* br, maac_u8 bits);

MAAC_PRIVATE
void
maac_bitreader_discard(maac_bitreader* br, maac_u8 bits);

MAAC_PRIVATE
maac_u32
maac_bitreader_read(maac_bitreader* br, maac_u8 bits);

MAAC_PRIVATE
void
maac_bitreader_byte_align(maac_bitreader* br);

#endif /* INCLUDE_GUARD */
