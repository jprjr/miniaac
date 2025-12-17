/* SPDX-License-Identifier: 0BSD */

#include "raw_extras.h"
#include "maac_align.h"
#include "channel_config.h"

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_channels(const maac_raw* r) {
    return maac_channel_config_channels(r->channel_configuration);
}

maac_const
MAAC_PUBLIC
size_t
maac_raw_size(void) {
    return sizeof(maac_raw);
}

struct maac_raw_aligner {
    char c;
    maac_raw r;
};

maac_const
MAAC_PUBLIC
size_t
maac_raw_alignof(void) {
    return offsetof(struct maac_raw_aligner, r);
}

maac_pure
MAAC_PUBLIC
maac_raw* maac_raw_align(void* p) {
    return (maac_raw*)maac_align(p, offsetof(struct maac_raw_aligner, r));
}

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_state(const maac_raw* r) {
    return (maac_u32)r->state;
}

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_sf_index(const maac_raw* r) {
    return (maac_u32)r->sf_index;
}

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_ele_id(const maac_raw* r) {
    return (maac_u32)r->ele_id;
}

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_sample_rate(const maac_raw* r) {
    return (maac_u32)r->sample_rate;
}

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_num_out_channels(const maac_raw* r) {
    return (maac_u32)r->num_out_channels;
}

maac_pure
MAAC_PUBLIC
maac_u32 maac_raw_channel_configuration(const maac_raw* r) {
    return (maac_u32)r->channel_configuration;
}

maac_pure
MAAC_PUBLIC
const maac_channel* maac_raw_out_channels(const maac_raw* r) {
    return r->out_channels;
}

MAAC_PUBLIC
maac_fil*
maac_raw_fil(maac_raw* r) {
    return &r->ele.fil;
}

MAAC_PUBLIC
maac_sce*
maac_raw_sce(maac_raw* r) {
    return &r->ele.sce;
}

MAAC_PUBLIC
maac_cpe*
maac_raw_cpe(maac_raw* r) {
    return &r->ele.cpe;
}

static const char* MAAC_RAW_DATA_BLOCK_ID_INVALID_STR = "INVALID";
static size_t MAAC_RAW_DATA_BLOCK_ID_INVALID_LEN = sizeof("INVALID") - 1;

static const char* const maac_raw_data_block_id_str_tbl[8] = {
    "SCE",
    "CPE",
    "CCE",
    "LFE",
    "DSE",
    "PCE",
    "FIL",
    "END"
};

static const size_t maac_raw_data_block_id_len_tbl[8] = {
    sizeof("SCE") - 1,
    sizeof("CPE") - 1,
    sizeof("CCE") - 1,
    sizeof("LFE") - 1,
    sizeof("DSE") - 1,
    sizeof("PCE") - 1,
    sizeof("FIL") - 1,
    sizeof("END") - 1
};

maac_const
MAAC_PUBLIC
const char*
maac_raw_data_block_id_name(const maac_u32 id) {
    return id > MAAC_RAW_DATA_BLOCK_ID_END ?
      MAAC_RAW_DATA_BLOCK_ID_INVALID_STR
      :
      maac_raw_data_block_id_str_tbl[id];
}

maac_const
MAAC_PUBLIC
size_t
maac_raw_data_block_id_name_len(const maac_u32 id) {
    return id > MAAC_RAW_DATA_BLOCK_ID_END ?
      MAAC_RAW_DATA_BLOCK_ID_INVALID_LEN
      :
      maac_raw_data_block_id_len_tbl[id];
}

MAAC_PUBLIC
void
maac_raw_set_out_channels(maac_raw* r, maac_channel* ch) {
    r->out_channels = ch;
}

MAAC_PUBLIC
void
maac_raw_set_num_out_channels(maac_raw* r, maac_u32 n) {
    r->num_out_channels = n;
}
