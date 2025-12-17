/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_ICS_INFO_INCLUDE_GUARD
#define MAAC_ICS_INFO_INCLUDE_GUARD

#include "maac_common.h"
#include "bitreader.h"
#include "bitfield.h"

enum MAAC_ICS_INFO_STATE {
    MAAC_ICS_INFO_STATE_RESERVED_BIT                   = 0,
    MAAC_ICS_INFO_STATE_WINDOW_SEQUENCE                = 1,
    MAAC_ICS_INFO_STATE_WINDOW_SHAPE                   = 2,
    MAAC_ICS_INFO_STATE_MAX_SFB                        = 3,
    MAAC_ICS_INFO_STATE_SCALE_FACTOR_GROUPING          = 4,
    MAAC_ICS_INFO_STATE_PREDICTOR_DATA_PRESENT         = 5,
#if MAAC_ENABLE_MAINPROFILE
    MAAC_ICS_INFO_STATE_PREDICTOR_RESET                = 6,
    MAAC_ICS_INFO_STATE_PREDICTOR_RESET_GROUP_NUMBER   = 7,
    MAAC_ICS_INFO_STATE_PREDICTION_USED                = 8,
    MAAC_ICS_INFO_STATE_LAST = MAAC_ICS_INFO_STATE_PREDICTION_USED
#else
    MAAC_ICS_INFO_STATE_LAST = MAAC_ICS_INFO_STATE_PREDICTOR_DATA_PRESENT
#endif
};

typedef enum MAAC_ICS_INFO_STATE MAAC_ICS_INFO_STATE;

struct maac_ics_info {
    MAAC_ICS_INFO_STATE state;
    MAAC_BITFIELD(maac_u8,window_sequence,2);
    MAAC_BITFIELD(maac_u8,window_shape,1);
    MAAC_BITFIELD(maac_u8,max_sfb,6);
    MAAC_BITFIELD(maac_u8,scale_factor_grouping,7);
    MAAC_BITFIELD(maac_u8,predictor_data_present,1);
#if MAAC_ENABLE_MAINPROFILE
    MAAC_BITFIELD(maac_u8,predictor_reset,1);
    MAAC_BITFIELD(maac_u8,predictor_reset_group_number,5);
    maac_u32 prediction_used[2];

    maac_u8 _sfb;
#endif
};

typedef struct maac_ics_info maac_ics_info;

MAAC_CDECLS_BEGIN

MAAC_PUBLIC
void
maac_ics_info_init(maac_ics_info* ics_info);

MAAC_PUBLIC
MAAC_RESULT
maac_ics_info_parse(maac_ics_info* maac_restrict ics_info, maac_bitreader* maac_restrict br, const maac_u32 sf_index);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
