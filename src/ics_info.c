/* SPDX-License-Identifier: 0BSD */
#include "ics_info.h"
#include "bitreader_private.h"

#include "maac_memset.h"
#include "maac_consts.h"
#include "maac_min.h"

MAAC_PUBLIC
void
maac_ics_info_init(maac_ics_info* ics_info) {
    /* TODO - remove memset with garbage data once we verify this works correctly with mostly garbage data */
    maac_memset(ics_info, 0xa1, sizeof *ics_info);
    ics_info->state = MAAC_ICS_INFO_STATE_RESERVED_BIT;
}

#if MAAC_ENABLE_MAINPROFILE
/* Not sure if I'll ever go full main-profile tbh */

static const maac_u8 maac_pred_sfb_max_tbl[16] = {
    /* 0x00 (96000Hz)  */ 33,
    /* 0x01 (88200Hz)  */ 33,
    /* 0x02 (64000Hz)  */ 38,
    /* 0x03 (48000Hz)  */ 40,
    /* 0x04 (44100Hz)  */ 40,
    /* 0x05 (32000Hz)  */ 40,
    /* 0x06 (24000Hz)  */ 41,
    /* 0x07 (22050Hz)  */ 41,
    /* 0x08 (16000Hz)  */ 37,
    /* 0x09 (12000Hz)  */ 37,
    /* 0x0a (11025Hz)  */ 37,
    /* 0x0b  (8000Hz)  */ 34,
    /* 0x00 (reserved) */ 0,
    /* 0x00 (reserved) */ 0,
    /* 0x00 (reserved) */ 0,
    /* 0x00 (reserved) */ 0
};


maac_const
static
maac_u32
maac_pred_sfb_max(const maac_u32 sf_index) {
    return sf_index > 15 ? 0 : pred_sfb_max_tbl[sf_index];
}
#endif

MAAC_PUBLIC
MAAC_RESULT
maac_ics_info_parse(maac_ics_info* maac_restrict info, maac_bitreader* maac_restrict br, const maac_u32 sf_index) {
    MAAC_RESULT res;
    maac_u8 bits;
#if MAAC_ENABLE_MAINPROFILE
    maac_u8 pred_max;
#else
    (void)sf_index;
#endif

    switch(info->state) {
        case MAAC_ICS_INFO_STATE_RESERVED_BIT: {
            if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
            maac_bitreader_discard(br,1);
            info->state = MAAC_ICS_INFO_STATE_WINDOW_SEQUENCE;
        }
        /* fall-through */
        case MAAC_ICS_INFO_STATE_WINDOW_SEQUENCE: {
            if( (res = maac_bitreader_fill(br, 2)) != MAAC_OK) return res;
            info->window_sequence = (maac_u8)maac_bitreader_read(br,2);
            info->state = MAAC_ICS_INFO_STATE_WINDOW_SHAPE;
        }
        /* fall-through */
        case MAAC_ICS_INFO_STATE_WINDOW_SHAPE: {
            if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
            info->window_shape = (maac_u8)maac_bitreader_read(br,1);
            info->state = MAAC_ICS_INFO_STATE_MAX_SFB;
        }
        /* fall-through */
        case MAAC_ICS_INFO_STATE_MAX_SFB: {
            bits = info->window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ? 4 : 6;
            if( (res = maac_bitreader_fill(br, bits)) != MAAC_OK) return res;
            info->max_sfb = (maac_u8)maac_bitreader_read(br,bits);
            if(info->window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT) {
                info->state = MAAC_ICS_INFO_STATE_SCALE_FACTOR_GROUPING;
                goto maac_ics_info_state_scale_factor_grouping;
            }
            info->scale_factor_grouping = 0x7f;
            info->state = MAAC_ICS_INFO_STATE_PREDICTOR_DATA_PRESENT;
            goto maac_ics_info_state_predictor_data_present;
        }

        case MAAC_ICS_INFO_STATE_SCALE_FACTOR_GROUPING: {
            maac_ics_info_state_scale_factor_grouping:
            if( (res = maac_bitreader_fill(br, 7)) != MAAC_OK) return res;
            info->scale_factor_grouping = maac_bitreader_read(br,7);
            break;
        }

        case MAAC_ICS_INFO_STATE_PREDICTOR_DATA_PRESENT: {
            maac_ics_info_state_predictor_data_present:
            if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
            info->predictor_data_present = maac_bitreader_read(br,1);
            if(info->predictor_data_present) {
#if MAAC_ENABLE_MAINPROFILE
                info->_sfb = 0;
                info->prediction_used[0] = 0;
                info->prediction_used[1] = 0;
                info->state = MAAC_ICS_INFO_STATE_PREDICTOR_RESET;
                goto maac_ics_info_state_predictor_reset;
#else
                return MAAC_PREDICTOR_DATA_NOT_IMPLEMENTED;
#endif
            }
            break;
        }

#if MAAC_ENABLE_MAINPROFILE
        case MAAC_ICS_INFO_STATE_PREDICTOR_RESET: {
            maac_ics_info_state_predictor_reset:
            if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
            info->predictor_reset = maac_bitreader_read(br,1);
            if(info->predictor_reset) {
                info->state = MAAC_ICS_INFO_STATE_PREDICTOR_RESET_GROUP_NUMBER;
                goto maac_ics_info_state_predictor_reset_group_number;
            }
            info->state = MAAC_ICS_INFO_STATE_PREDICTION_USED;
            goto maac_ics_info_state_prediction_used;
        }

        case MAAC_ICS_INFO_STATE_PREDICTOR_RESET_GROUP_NUMBER: {
            maac_ics_info_state_predictor_reset_group_number:
            if( (res = maac_bitreader_fill(br, 5)) != MAAC_OK) return res;
            info->predictor_reset_group_number = maac_bitreader_read(br,5);
            info->state = MAAC_ICS_INFO_STATE_PREDICTION_USED;
            goto maac_ics_info_state_prediction_used;
        }

        case MAAC_ICS_INFO_STATE_PREDICTION_USED: {
            maac_ics_info_state_prediction_used:
            pred_max = maac_min(info->max_sfb,maac_pred_sfb_max(sf_index));
            while(info->_sfb < pred_max) {
                if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
                info->prediction_used[info->_sfb / 32] |= ( (maac_bitreader_read(br, 1)) << (info->_sfb % 32) );
                info->_sfb++;
            }
            break;
        }
#endif
    }

    info->state = MAAC_ICS_INFO_STATE_RESERVED_BIT;
    return MAAC_OK;
}
