/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_CHANNEL_INCLUDE_GUARD
#define MAAC_CHANNEL_INCLUDE_GUARD

/* tracks what's needed to decode a single channel */

#include "maac_common.h"

struct maac_channel {
    maac_flt samples[3072];
      /* 0    -> 1023: audio samples
         1024 -> 2047: space for IMDCT, maybe SBR samples in the future?
         2048 -> 3071: overlap from previous frame.
       a heads-up that the audio samples in 0-1023 have not been
       clamped and may be outside the signed 16-bit range, you'll want to
       make sure they're in range before writing them out. */
    maac_u8 window_shape_prev;
    maac_u16 n_samples; /* the number of samples */
    maac_u16 _n; /* used for tracking samples written out */
};

typedef struct maac_channel maac_channel;

MAAC_CDECLS_BEGIN

MAAC_PUBLIC
void
maac_channel_init(maac_channel *ch);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
