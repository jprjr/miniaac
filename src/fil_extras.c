/* SPDX-License-Identifier: 0BSD */
#include "fil_extras.h"

#include "maac_align.h"

maac_const
MAAC_PUBLIC
size_t
maac_fil_size(void) {
    return sizeof(maac_fil);
}

struct maac_fil_aligner {
    char c;
    maac_fil f;
};

maac_const
MAAC_PUBLIC
size_t
maac_fil_alignof(void) {
    return offsetof(struct maac_fil_aligner, f);
}

maac_pure
MAAC_PUBLIC
maac_fil*
maac_fil_align(void* p) {
    return (maac_fil*)maac_align(p, offsetof(struct maac_fil_aligner, f));
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_fil_extension_type(const maac_fil* f) {
    return f->extension_type;
}

static const char* MAAC_FIL_EXT_INVALID_STR = "INVALID";
static const size_t MAAC_FIL_EXT_INVALID_LEN = sizeof("INVALID") - 1;

static const char* const maac_fil_extension_type_str_tbl[16] = {
    /* 0x00 */ "FILL",
    /* 0x01 */ "FILL_DATA",
    /* 0x02 */ "INVALID",
    /* 0x03 */ "INVALID",
    /* 0x04 */ "INVALID",
    /* 0x05 */ "INVALID",
    /* 0x06 */ "INVALID",
    /* 0x07 */ "INVALID",
    /* 0x08 */ "INVALID",
    /* 0x09 */ "INVALID",
    /* 0x0a */ "INVALID",
    /* 0x0b */ "DYNAMIC_RANGE",
    /* 0x0c */ "INVALID",
    /* 0x0d */ "SBR_DATA",
    /* 0x0e */ "SBR_DATA_CRC",
    /* 0x00 */ "INVALID"
};

static const size_t maac_fil_extension_type_len_tbl[16] = {
    /* 0x00 */ sizeof("FILL") - 1,
    /* 0x01 */ sizeof("FILL_DATA") - 1,
    /* 0x02 */ sizeof("INVALID") - 1,
    /* 0x03 */ sizeof("INVALID") - 1,
    /* 0x04 */ sizeof("INVALID") - 1,
    /* 0x05 */ sizeof("INVALID") - 1,
    /* 0x06 */ sizeof("INVALID") - 1,
    /* 0x07 */ sizeof("INVALID") - 1,
    /* 0x08 */ sizeof("INVALID") - 1,
    /* 0x09 */ sizeof("INVALID") - 1,
    /* 0x0a */ sizeof("INVALID") - 1,
    /* 0x0b */ sizeof("DYNAMIC_RANGE") - 1,
    /* 0x0c */ sizeof("INVALID") - 1,
    /* 0x0d */ sizeof("SBR_DATA") - 1,
    /* 0x0e */ sizeof("SBR_DATA_CRC") - 1,
    /* 0x00 */ sizeof("INVALID") - 1
};

maac_const
MAAC_PUBLIC
const char*
maac_fil_extension_type_name(const maac_u32 id) {
    return id > 15 ? MAAC_FIL_EXT_INVALID_STR : 
        maac_fil_extension_type_str_tbl[id];
}

maac_const
MAAC_PUBLIC
size_t
maac_fil_extension_type_name_len(const maac_u32 id) {
    return id > 15 ? MAAC_FIL_EXT_INVALID_LEN : 
        maac_fil_extension_type_len_tbl[id];
}

