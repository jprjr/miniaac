#include "channel.h"

static const maac_u32 channel_maps[8][8] = {
    /* invalid */
    { MAAC_U32_C(~0) },

    /* mono */
    { MAAC_U32_C(0) },

    /* stereo */
    { MAAC_U32_C(0), MAAC_U32_C(1) },

    /* AAC: FC, FL, FR */
    /* WAVE: FL, FR, FC */
    { MAAC_U32_C(2), MAAC_U32_C(0), MAAC_U32_C(1) },

    /* AAC: FC, FL, FR, BC */
    /* WAVE: FL, FR, FC */
    { MAAC_U32_C(2), MAAC_U32_C(0), MAAC_U32_C(1) },

    /* AAC: FC, FL, FR, BL, BR */
    /* WAVE: FL, FR, FC, BL, BR */
    { MAAC_U32_C(2), MAAC_U32_C(0), MAAC_U32_C(1), MAAC_U32_C(3), MAAC_U32_C(4) },

    /* AAC: FC, FL, FR, BL, BR, LFE */
    /* WAVE: FL, FR, FC, LFE, BL, BR */
    { MAAC_U32_C(2), MAAC_U32_C(0), MAAC_U32_C(1), MAAC_U32_C(4), MAAC_U32_C(5), MAAC_U32_C(3) },

    /* AAC: FC, FL, FR, SL, SR, BL, BR, LFE */
    /* WAVE: FL, FR, FC, LFE, BL, BR, SL, SR */
    { MAAC_U32_C(2), MAAC_U32_C(0), MAAC_U32_C(1), MAAC_U32_C(6), MAAC_U32_C(7), MAAC_U32_C(4), MAAC_U32_C(5), MAAC_U32_C(3) }
};

maac_u32 channel_map(maac_u8 ch_layout, maac_u32 ch_number) {
    return channel_maps[ch_layout][ch_number];
}

#define FRONT_LEFT MAAC_U32_C(0x01)
#define FRONT_RIGHT MAAC_U32_C(0x02)
#define FRONT_CENTER MAAC_U32_C(0x04)
#define LFE MAAC_U32_C(0x08)
#define BACK_LEFT MAAC_U32_C(0x10)
#define BACK_RIGHT MAAC_U32_C(0x20)
#define BACK_CENTER MAAC_U32_C(0x100)
#define SIDE_LEFT MAAC_U32_C(0x200)
#define SIDE_RIGHT MAAC_U32_C(0x400)

maac_u32 channel_mask(maac_u8  ch_layout) {
    switch(ch_layout) {
        case 1: return FRONT_CENTER;
        case 2: return FRONT_LEFT | FRONT_RIGHT;
        case 3: return FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER;
        case 4: return FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER | BACK_CENTER;
        case 5: return FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER | BACK_LEFT | BACK_RIGHT;
        case 6: return FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER | LFE | BACK_LEFT | BACK_RIGHT;
        case 7: return FRONT_LEFT | FRONT_RIGHT | FRONT_CENTER | LFE | BACK_LEFT | BACK_RIGHT | SIDE_LEFT | SIDE_RIGHT;
        default: break;
    }
    return MAAC_U32_C(~0);
}

