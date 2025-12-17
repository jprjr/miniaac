/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_CHANNEL_EXTRAS_INCLUDE_GUARD
#define MAAC_CHANNEL_EXTRAS_INCLUDE_GUARD

#include "channel.h"

MAAC_CDECLS_BEGIN

maac_const
MAAC_PUBLIC
size_t
maac_channel_size(void);

maac_const
MAAC_PUBLIC
size_t
maac_channel_alignof(void);

maac_pure
MAAC_PUBLIC
maac_channel*
maac_channel_align(void*);

/* returns a pointer to the raw sample values,
which will be an maac_flt type in the range of
(about) S16_MIN <=> S16_MAX. These values have not been
clamped, you'll want to make sure you clamp them! */
maac_pure
MAAC_PUBLIC
maac_flt* maac_channel_samples(maac_channel*);

/* returns the number of samples in the channel - specifically
the number of samples minus the internal counter used
by maac_samples_float and maac_samples_16 */
maac_pure
MAAC_PUBLIC
maac_u32 maac_channel_n_samples(const maac_channel*);

/* write up to lim samples to floats in the range [-1.0, +1.0),
clamped.  This will increment an internal counter so you can write
samples out in chunks */
MAAC_PUBLIC
void
maac_channel_samples_float(maac_channel*, float*, maac_u32 lim);

/* similar to above but writes to signed 16-bit values, in
the range [-32786,32767].  Also increments the internal counter */
MAAC_PUBLIC
void
maac_channel_samples_s16(maac_channel*, maac_s16*, maac_u32 lim);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
