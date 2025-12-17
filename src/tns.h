/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_TEMPORAL_NOISE_SHAPING_INCLUDE_GUARD
#define MAAC_TEMPORAL_NOISE_SHAPING_INCLUDE_GUARD

#include "maac_common.h"
#include "maac_consts.h"
#include "bitreader.h"
#include "bitfield.h"

#include "ics_info.h"
#include "maac_float.h"

enum MAAC_TNS_STATE {
    MAAC_TNS_STATE_N_FILT           = 0,
    MAAC_TNS_STATE_COEF_RES         = 1,
    MAAC_TNS_STATE_LENGTH           = 2,
    MAAC_TNS_STATE_ORDER            = 3,
    MAAC_TNS_STATE_DIRECTION        = 4,
    MAAC_TNS_STATE_COEF_COMPRESS    = 5,
    MAAC_TNS_STATE_COEF             = 6
};

typedef enum MAAC_TNS_STATE MAAC_TNS_STATE;

struct maac_tns_filter {
    MAAC_BITFIELD(maac_u8, length, 6);
    MAAC_BITFIELD(maac_u8, direction, 1);
    MAAC_BITFIELD(maac_u8, coef_compress, 1);
    MAAC_BITFIELD(maac_u8, order, 5);
    /* we use 4 bits per coeff */
    maac_u8 coef[ (MAAC_TNS_TOTAL_ORDER/2) + (MAAC_TNS_TOTAL_ORDER % 2)];
};
typedef struct maac_tns_filter maac_tns_filter;

struct maac_tns_window {
    MAAC_BITFIELD(maac_u8, n_filt, 2);
    MAAC_BITFIELD(maac_u8, coef_res, 1);
    maac_tns_filter filt[3];
};
typedef struct maac_tns_window maac_tns_window;

struct maac_tns {
    MAAC_TNS_STATE state;
    maac_tns_window window[8];

    maac_u8 _g;
    maac_u8 _k;
    maac_u8 _i;
};
typedef struct maac_tns maac_tns;

struct maac_tns_params {
    const maac_ics_info* info;
    maac_u8 sf_index;
};
typedef struct maac_tns_params maac_tns_params;

MAAC_PRIVATE
MAAC_RESULT
maac_tns_parse(maac_tns* maac_restrict tns, maac_bitreader* maac_restrict br, maac_u8 window_sequence);

MAAC_PRIVATE
void
maac_tns_process(maac_tns* maac_restrict tns, maac_flt* maac_restrict samples, const maac_tns_params* maac_restrict p);

#endif /* INCLUDE_GUARD */
