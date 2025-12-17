/* SPDX-License-Identifier: 0BSD */
#include "ics_info.h"

#include "maac_align.h"
#include "maac_consts.h"

maac_const
MAAC_PUBLIC
size_t
maac_ics_info_size(void) {
    return sizeof(maac_ics_info);
}

struct maac_ics_info_aligner {
    char c;
    maac_ics_info ics_info;
};

maac_const
MAAC_PUBLIC
size_t
maac_ics_info_alignof(void) {
    return offsetof(struct maac_ics_info_aligner, ics_info);
}

maac_pure
MAAC_PUBLIC
maac_ics_info*
maac_ics_info_align(void* p) {
    return (maac_ics_info*)maac_align(p, offsetof(struct maac_ics_info_aligner, ics_info));
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_state(const maac_ics_info* ics_info) {
    return (maac_u32)ics_info->state;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_max_sfb(const maac_ics_info* ics_info) {
    return (maac_u32)ics_info->max_sfb;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_predictor_data_present(const maac_ics_info* ics_info) {
    return (maac_u32)ics_info->predictor_data_present;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_scale_factor_grouping(const maac_ics_info* ics_info) {
    return (maac_u32)ics_info->scale_factor_grouping;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_num_window_groups(const maac_ics_info* ics_info) {
    maac_u32 c = 1;
    maac_u32 pc = 0;
    maac_u8 sfg = ics_info->scale_factor_grouping;

    if(ics_info->window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT) {

        while(sfg) {
            sfg &= sfg - 1;
            ++pc;
        }
        c += 7 - pc;
    }

    return c;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_window_sequence(const maac_ics_info* ics_info) {
    return (maac_u32)ics_info->window_sequence;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_window_shape(const maac_ics_info* ics_info) {
    return (maac_u32)ics_info->window_shape;
}

#if MAAC_ENABLE_MAINPROFILE
maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_predictor_reset(const maac_ics_info* ics_info) {
    return (maac_u32)ics_info->predictor_reset;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_predictor_reset_group_number(const maac_ics_info* ics_info) {
    return (maac_u32)ics_info->predictor_reset_group_number;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_info_prediction_used(const maac_ics_info* info, maac_u32 sfb) {
    if(sfb > 63) {
        return 0;
    }
    return (info->prediction_used[sfb/32] >> (sfb % 32)) & 0x01;
}

#endif

static const char* MAAC_ICS_INFO_STATE_INVALID_STR = "INVALID";
static const size_t MAAC_ICS_INFO_STATE_INVALID_LEN = sizeof("INVALID") - 1;

static const char* const maac_ics_info_state_str_tbl[MAAC_ICS_INFO_STATE_LAST+1] = {
    "RESERVED_BIT",
    "WINDOW_SEQUENCE",
    "WINDOW_SHAPE",
    "MAX_SFB",
    "SCALE_FACTOR_GROUPING",
    "PREDICTOR_DATA_PRESENT"
#if MAAC_ENABLE_MAINPROFILE
    ,"PREDICTOR_RESET",
    "PREDICTOR_RESET_GROUP_NUMBER",
    "PREDICTION_USED"
#endif
};

static const size_t maac_ics_info_state_len_tbl[MAAC_ICS_INFO_STATE_LAST+1] = {
    sizeof("RESERVED_BIT") - 1,
    sizeof("WINDOW_SEQUENCE") - 1,
    sizeof("WINDOW_SHAPE") - 1,
    sizeof("MAX_SFB") - 1,
    sizeof("SCALE_FACTOR_GROUPING") - 1,
    sizeof("PREDICTOR_DATA_PRESENT") - 1
#if MAAC_ENABLE_MAINPROFILE
    ,sizeof("PREDICTOR_RESET") - 1,
    sizeof("PREDICTOR_RESET_GROUP_NUMBER") - 1,
    sizeof("PREDICTION_USED") - 1
#endif
};

maac_const
MAAC_PUBLIC
const char*
maac_ics_info_state_name(const maac_u32 state) {
    return state > MAAC_ICS_INFO_STATE_LAST ?
      MAAC_ICS_INFO_STATE_INVALID_STR
      :
      maac_ics_info_state_str_tbl[state];
}

maac_const
MAAC_PUBLIC
size_t
maac_ics_info_state_name_len(const maac_u32 state) {
    return state > MAAC_ICS_INFO_STATE_LAST ?
      MAAC_ICS_INFO_STATE_INVALID_LEN
      :
      maac_ics_info_state_len_tbl[state];
}

