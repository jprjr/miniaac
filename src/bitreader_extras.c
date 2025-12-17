/* SPDX-License-Identifier: 0BSD */
#include "bitreader_extras.h"
#include "maac_align.h"

maac_const
MAAC_PUBLIC
size_t
maac_bitreader_size(void) {
    return sizeof(maac_bitreader);
}

struct maac_bitreader_aligner {
    char c;
    maac_bitreader b;
};

maac_const
MAAC_PUBLIC
size_t
maac_bitreader_alignof(void) {
    return offsetof(struct maac_bitreader_aligner, b);
}

MAAC_PUBLIC
maac_bitreader*
maac_bitreader_align(void* ptr) {
    return (maac_bitreader*)maac_align(ptr, offsetof(struct maac_bitreader_aligner, b));
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_bitreader_avail(const maac_bitreader* br) {
    return br->len - br->pos;
}

MAAC_PUBLIC
void
maac_bitreader_set_data(maac_bitreader* br, const maac_u8* data) {
    br->data = data;
}

MAAC_PUBLIC
void
maac_bitreader_set_pos(maac_bitreader* br, maac_u32 pos) {
    br->pos = pos;
}

MAAC_PUBLIC
void
maac_bitreader_set_len(maac_bitreader* br, maac_u32 len) {
    br->len = len;
}

/* getter functions */

maac_pure
MAAC_PUBLIC
const maac_u8*
maac_bitreader_data(const maac_bitreader* br) {
    return br->data;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_bitreader_pos(const maac_bitreader* br) {
    return br->pos;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_bitreader_len(const maac_bitreader* br) {
    return br->len;
}

