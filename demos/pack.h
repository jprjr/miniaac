/* SPDX-License-Identifier: 0BSD */
#include "maac.h"

static maac_inline void
pack_u32le(maac_u8* output, maac_u32 n) {
    output[0] = (maac_u8)(n & 0xFF);
    output[1] = (maac_u8)(n >> 8  );
    output[2] = (maac_u8)(n >> 16 );
    output[3] = (maac_u8)(n >> 24 );
}

static maac_inline void
pack_s32le(maac_u8* output, maac_s32 n) {
    pack_u32le(output,(maac_u32)n);
}


static maac_inline void
pack_u16le(maac_u8* output, maac_u16 n) {
    output[0] = (maac_u8)(n & 0xFF);
    output[1] = (maac_u8)(n >> 8  );
}

static maac_inline void
pack_s16le(maac_u8* output, maac_s16 n) {
    pack_u16le(output, (maac_u16)n);
}

