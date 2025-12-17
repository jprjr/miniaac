/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_BITREADER_INCLUDE_GUARD
#define MAAC_BITREADER_INCLUDE_GUARD

#include "maac_common.h"

struct maac_bitreader {
    const maac_u8* data;
    maac_u32 val;
    maac_u32 pos;
    maac_u32 len;
    maac_u8 bits;
};

typedef struct maac_bitreader maac_bitreader;

MAAC_CDECLS_BEGIN

MAAC_PUBLIC
void
maac_bitreader_init(maac_bitreader* br);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
