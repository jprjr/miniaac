/* SPDX-License-Identifier: 0BSD */
#include "channel_extras.h"

#include "maac_align.h"
#include "maac_clamp.h"

maac_const
MAAC_PUBLIC
size_t
maac_channel_size(void) {
    return sizeof(maac_channel);
}

struct maac_channel_aligner {
    char c;
    maac_channel ch;
};

maac_const
MAAC_PUBLIC
size_t
maac_channel_alignof(void) {
    return offsetof(struct maac_channel_aligner, ch);
}

maac_pure
MAAC_PUBLIC
maac_channel* maac_channel_align(void* p) {
    return (maac_channel*)maac_align(p, offsetof(struct maac_channel_aligner, ch));
}

maac_pure
MAAC_PUBLIC
maac_flt* maac_channel_samples(maac_channel* ch) {
    return &ch->samples[0];
}

maac_pure
MAAC_PUBLIC
maac_u32 maac_channel_n_samples(const maac_channel* ch) {
    return ch->n_samples - ch->_n;
}

static const maac_flt maac_channel_divisor = MAAC_FLT_C(1.0) / MAAC_FLT_C(32768.0);
static const maac_f32 maac_channel_max = MAAC_F32_C(32767.0) / MAAC_F32_C(32768.0);

MAAC_PUBLIC
void
maac_channel_samples_float(maac_channel* ch, float* s, maac_u32 lim) {
    maac_u32 i;
    maac_flt t;
    if(lim > (maac_u32)(ch->n_samples - ch->_n)) {
        lim = ch->n_samples - ch->_n;
    }

    for(i=0;i<lim;i++) {
        t = ch->samples[ch->_n++] * maac_channel_divisor;
        t = maac_clamp(t, MAAC_FLT_C(-1.0), maac_flt_cast(maac_channel_max));
        s[i] = (float)t;
    }
}

MAAC_PUBLIC
void
maac_channel_samples_s16(maac_channel* ch, maac_s16* s, maac_u32 lim) {
    maac_u32 i;
    maac_s32 t;
    if(lim > (maac_u32)(ch->n_samples - ch->_n)) {
        lim = ch->n_samples - ch->_n;
    }

    for(i=0;i<lim;i++) {
        t = (maac_s32)ch->samples[ch->_n++];
        t = maac_clamp(t, MAAC_S32_C(-32768), MAAC_S32_C(32767));
        s[i] = (maac_s16)t;
    }
}
