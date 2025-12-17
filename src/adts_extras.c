/* SPDX-License-Identifier: 0BSD */
#include "adts_extras.h"

#include "maac_align.h"
#include "maac_frequency.h"

maac_const
MAAC_PUBLIC
size_t
maac_adts_size(void) {
    return sizeof(maac_adts);
}

struct maac_adts_aligner {
    char c;
    maac_adts a;
};

maac_const
MAAC_PUBLIC
size_t
maac_adts_alignof(void) {
    return offsetof(struct maac_adts_aligner, a);
}

maac_pure
MAAC_PUBLIC
maac_adts* maac_adts_align(void* p) {
    return (maac_adts*)maac_align(p, offsetof(struct maac_adts_aligner, a));
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_tolerance(const maac_adts* a) {
    return a->tolerance;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_mpeg_version(const maac_adts* a) {
    return a->fixed_header.version;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_layer(const maac_adts* a) {
    return a->fixed_header.layer;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_protection_absent(const maac_adts* a) {
    return a->fixed_header.protection_absent;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_profile(const maac_adts* a) {
    return a->fixed_header.profile + 1;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_frequency_index(const maac_adts* a) {
    return a->fixed_header.sampling_frequency_index;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_sample_rate(const maac_adts* a) {
    return maac_sampling_frequency((maac_u32)a->fixed_header.sampling_frequency_index);
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_channel_configuration(const maac_adts* a) {
    return a->fixed_header.channel_configuration;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_channels(const maac_adts* a) {
    switch(a->fixed_header.channel_configuration) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
        case 5: return 5;
        case 6: return 6;
        case 7: return 8;
        default: break;
    }
    return 0;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_original_copy(const maac_adts* a) {
    return a->fixed_header.original_copy;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_home(const maac_adts* a) {
    return a->fixed_header.home;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_copyright_id_bit(const maac_adts* a) {
    return a->variable_header.copyright_id_bit;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_copyright_id_start(const maac_adts* a) {
    return a->variable_header.copyright_id_start;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_frame_length(const maac_adts* a) {
    return a->variable_header.frame_length;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_buffer_fullness(const maac_adts* a) {
    return a->variable_header.buffer_fullness;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_adts_raw_data_blocks(const maac_adts* a) {
    return a->variable_header.raw_data_blocks + 1;
}

MAAC_PUBLIC
void
maac_adts_set_tolerance(maac_adts* a, maac_u32 tolerance) {
    a->tolerance = tolerance;
}

MAAC_PUBLIC
void
maac_adts_set_num_out_channels(maac_adts* a, maac_u32 num) {
    a->raw.num_out_channels = num;
}

MAAC_PUBLIC
void
maac_adts_set_out_channels(maac_adts* a, maac_channel* ch) {
    a->raw.out_channels = ch;
}
