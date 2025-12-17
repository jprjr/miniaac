/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_SINGLE_CHANNEL_ELEMENT_INCLUDE_GUARD
#define MAAC_SINGLE_CHANNEL_ELEMENT_INCLUDE_GUARD

#include "maac_common.h"
#include "bitreader.h"
#include "ics.h"

enum MAAC_SCE_STATE {
    MAAC_SCE_STATE_TAG  = 0,
    MAAC_SCE_STATE_ICS  = 1
};

typedef enum MAAC_SCE_STATE MAAC_SCE_STATE;

struct maac_sce {
    MAAC_SCE_STATE state;
    maac_u8 element_instance_tag;
    maac_ics ics;
};

typedef struct maac_sce maac_sce;

struct maac_sce_decode_params {
    maac_u32 sf_index;
    maac_channel* ch;
    maac_u32* rand_state;
};
typedef struct maac_sce_decode_params maac_sce_decode_params;

MAAC_CDECLS_BEGIN

MAAC_PUBLIC
void
maac_sce_init(maac_sce* s);

MAAC_PUBLIC
MAAC_RESULT
maac_sce_decode(maac_sce* maac_restrict s, maac_bitreader* maac_restrict br, const maac_sce_decode_params* maac_restrict p);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
