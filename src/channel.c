/* SPDX-License-Identifier: 0BSD */
#include "channel.h"

#include "maac_memset.h"

MAAC_PUBLIC
void
maac_channel_init(maac_channel *ch) {
    maac_memset(ch, 0, sizeof *ch);
}

