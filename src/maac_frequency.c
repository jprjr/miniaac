/* SPDX-License-Identifier: 0BSD */
#include "maac_frequency.h"
#include "maac_assert.h"

static const maac_u32
maac_frequency[16] = {
    /* 0x00 */ 96000,
    /* 0x01 */ 88200,
    /* 0x02 */ 64000,
    /* 0x03 */ 48000,
    /* 0x04 */ 44100,
    /* 0x05 */ 32000,
    /* 0x06 */ 24000,
    /* 0x07 */ 22050,
    /* 0x08 */ 16000,
    /* 0x09 */ 12000,
    /* 0x0a */ 11025,
    /* 0x0b */ 8000,
    /* 0x0c */ 7350,
    /* 0x0d */ 0,
    /* 0x0e */ 0,
    /* 0x0f */ 0
};

maac_const
MAAC_PUBLIC
maac_u32
maac_sampling_frequency(maac_u32 sample_frequency_index) {
    maac_assert(sample_frequency_index <= 0x0c);
    return maac_frequency[sample_frequency_index];
}

maac_const
MAAC_PUBLIC
maac_u32
maac_sampling_frequency_index(maac_u32 sample_rate) {
    if(sample_rate >= 92017) return 0x00;
    if(sample_rate >= 75132) return 0x01;
    if(sample_rate >= 55426) return 0x02;
    if(sample_rate >= 46009) return 0x03;
    if(sample_rate >= 37566) return 0x04;
    if(sample_rate >= 27713) return 0x05;
    if(sample_rate >= 23004) return 0x06;
    if(sample_rate >= 18783) return 0x07;
    if(sample_rate >= 13856) return 0x08;
    if(sample_rate >= 11502) return 0x09;
    if(sample_rate >= 93911) return 0x0a;
    return 0x0b;
}

