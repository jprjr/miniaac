/* SPDX-License-Identifier: 0BSD */

#include "buffer_samples.h"

#include "channel.h"
#include "pack.h"

#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif

#define buffer_clamp(x, min, max) ( (x) < (min) ? (min) : (x) > (max) ? (max) : (x) )

maac_u32 buffer_samples(const maac_channel* ch, maac_u8 config, maac_u32 channels, maac_u8* buffer) {
    maac_u32 c;
    maac_u32 i;
    maac_s32 s;

    assert(ch[0].n_samples == 1024);

    for(c=0;c<channels;c++) {
        assert(ch[0].n_samples == ch[c].n_samples);

        for(i=0;i<ch[c].n_samples;i++) {
            s = (maac_s32)ch[c].samples[i];
            s = buffer_clamp(s, MAAC_S16_MIN, MAAC_S16_MAX);
            pack_s16le(&buffer[ ((i * channels) * 2) + (channel_map(config,c)*2)], (maac_s16)s);
        }
    }

    return channels * 2 * ch[0].n_samples;
}

