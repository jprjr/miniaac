/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_CHANNEL_PAIR_ELEMENT_INCLUDE_GUARD
#define MAAC_CHANNEL_PAIR_ELEMENT_INCLUDE_GUARD

#include "maac_common.h"
#include "bitreader.h"
#include "ics.h"
#include "ics_info.h"

enum MAAC_CPE_STATE {
    MAAC_CPE_STATE_TAG             = 0,
    MAAC_CPE_STATE_COMMON_WINDOW   = 1,
    MAAC_CPE_STATE_ICS_INFO        = 2,
    MAAC_CPE_STATE_MS_MASK_PRESENT = 3,
    MAAC_CPE_STATE_MS_USED         = 4,
    MAAC_CPE_STATE_ICS_LEFT        = 5,
    MAAC_CPE_STATE_ICS_RIGHT       = 6
};

typedef enum MAAC_CPE_STATE MAAC_CPE_STATE;

struct maac_cpe {
    MAAC_CPE_STATE state;
    maac_u8 element_instance_tag;
    maac_u8 common_window;
    maac_u8 ms_mask_present;
    maac_u32 ms_used[4];
    maac_ics_info info;
    maac_ics ics_l;
    maac_ics ics_r;
    maac_u32* rand_state;

    maac_u8 _g;
    maac_u8 _sfb;
};

typedef struct maac_cpe maac_cpe;

struct maac_cpe_decode_params {
    maac_u32 sf_index;
    maac_channel* l;
    maac_channel* r;
    maac_u32* rand_state;
};
typedef struct maac_cpe_decode_params maac_cpe_decode_params;


MAAC_CDECLS_BEGIN

MAAC_PUBLIC
void
maac_cpe_init(maac_cpe* s);

MAAC_PUBLIC
MAAC_RESULT
maac_cpe_decode(maac_cpe* maac_restrict s, maac_bitreader* maac_restrict br, const maac_cpe_decode_params* maac_restrict p);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */

