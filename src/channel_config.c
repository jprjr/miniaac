/* SPDX-License-Identifier: 0BSD */

#include "channel_config.h"

maac_const
MAAC_PUBLIC
maac_u32 maac_channel_config_channels(maac_u32 channel_config) {
    switch(channel_config) {
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
