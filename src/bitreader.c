/* SPDX-License-Identifier: 0BSD */
#include "bitreader.h"
#include "bitreader_private.h"
#include "maac_memset.h"
#include "maac_assert.h"


MAAC_PUBLIC
void
maac_bitreader_init(maac_bitreader* br) {
    maac_memset(br, 0, sizeof *br);
}

MAAC_PRIVATE
MAAC_RESULT
maac_bitreader_fill(maac_bitreader* br, maac_u8 bits) {
    maac_u8 byte = 0;
    maac_assert(bits <= 32);

    if(bits == 0) return MAAC_OK;

    while(br->bits < bits && br->pos < br->len) {
        byte = br->data[br->pos++];
        br->val = (br->val << 8) | byte;
        br->bits += 8;
    }

    return (MAAC_RESULT) (br->bits >= bits);
}

MAAC_PRIVATE
maac_u32
maac_bitreader_peek(const maac_bitreader* br, maac_u8 bits) {
    uint32_t mask  = MAAC_U32_C(0xFFFFFFFF);
    uint32_t r = 0;
    maac_assert(bits <= 32);

    if(bits == 0) return r;
    mask >>= (32 - bits);

    bits = br->bits - bits;

    r = br->val >> bits & mask;

    return r;
}

MAAC_PRIVATE
void
maac_bitreader_discard(maac_bitreader* br, maac_u8 bits) {
    uint32_t imask = MAAC_U32_C(0xFFFFFFFF);
    maac_assert(bits <= 32);

    if(bits == 0) return;

    br->bits -= bits;
    if(br->bits == 0) {
        imask = 0;
    } else {
        imask >>= (32 - br->bits);
    }
    br->val &= imask;
    return;
}

MAAC_PRIVATE
maac_u32
maac_bitreader_read(maac_bitreader* br, maac_u8 bits) {
    uint32_t r = maac_bitreader_peek(br, bits);
    maac_bitreader_discard(br,bits);
    return r;
}

MAAC_PRIVATE
void
maac_bitreader_byte_align(maac_bitreader* br) {
    maac_bitreader_discard(br, br->bits % 8);
}

