/* SPDX-License-Identifier: 0BSD */
#ifdef __cplusplus
#include <cstdio>
#else
#include <stdio.h>
#endif

#include "maac.h"

/* a simple ADTS reader, used to demo using the raw interface, this
is only going to work with single-rdb ADTS files (which is nearly
all ADTS files) */

struct adts_header {
    maac_u16 syncword;
    maac_u8 version;
    maac_u8 layer;
    maac_u8 no_crc;
    maac_u8 profile;
    maac_u8 freq_index;
    maac_u8 priv;
    maac_u8 ch_config;
    maac_u8 original;
    maac_u8 home;
    maac_u8 copyright_bit;
    maac_u8 copyright_start;
    maac_u16 frame_length;
    maac_u8 buffer_fullness;
    maac_u8 frames;
    maac_u16 crc;
    maac_u8 aacAudioConfig[2]; /* synthesized from ADTS headers */
};

typedef struct adts_header adts_header;

int adts_grab_header(FILE* input, adts_header* header, int fatal);

