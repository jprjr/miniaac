/* SPDX-License-Identifier: 0BSD */
#include "raw.h"
#include "bitreader_private.h"
#include "maac_unreachable.h"
#include "maac_frequency.h"
#include "maac_rand.h"

MAAC_PUBLIC
MAAC_RESULT
maac_raw_config(maac_raw* r, const maac_u8* data, maac_u32 len) {
    maac_bitreader br;
    maac_u32 t;

    maac_bitreader_init(&br);
    br.data = data;
    br.len = len;

    if( (maac_bitreader_fill(&br,5)) != MAAC_OK) return MAAC_ERROR;
    t = maac_bitreader_read(&br, 5);
    if(t != 2) {
        return MAAC_UNSUPPORTED_AOT;
    }
    if( (maac_bitreader_fill(&br,4)) != MAAC_OK) return MAAC_ERROR;
    t = maac_bitreader_read(&br, 4);
    if(t == 15) {
        if( (maac_bitreader_fill(&br,24)) != MAAC_OK) return MAAC_ERROR;
        r->sample_rate = maac_bitreader_read(&br, 24);
        r->sf_index = maac_sampling_frequency_index(r->sample_rate);
    } else {
        r->sf_index = t;
        r->sample_rate = maac_sampling_frequency(r->sf_index);
    }
    if( (maac_bitreader_fill(&br, 4)) != MAAC_OK) return MAAC_ERROR;
    r->channel_configuration = maac_bitreader_read(&br,4);

    return MAAC_OK;
}

MAAC_PUBLIC
void
maac_raw_init(maac_raw* r) {
    r->state = MAAC_RAW_STATE_BLOCK_ID;
    r->sample_rate = 0;
    r->sf_index = 16; /* invalid index */
    r->ele_id = 0x08; /* invalid element */
    r->out_channels = NULL;
    r->num_out_channels = 0;
    r->channel_configuration = 16; /* invalid config */
    r->rand_state = maac_rand_seed();
    r->_c = 0;
}

MAAC_PUBLIC
MAAC_RESULT
maac_raw_sync(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br) {
    MAAC_RESULT res;

    switch(r->state) {
        case MAAC_RAW_STATE_BLOCK_ID: {
            if( (res = maac_bitreader_fill(br, 3)) != MAAC_OK) return res;
            r->ele_id = (maac_u8)maac_bitreader_read(br,3);
            switch(r->ele_id) {
                case MAAC_RAW_DATA_BLOCK_ID_SCE: {
                    r->state = MAAC_RAW_STATE_SCE;
                    maac_sce_init(&r->ele.sce);
                    break;
                }
                case MAAC_RAW_DATA_BLOCK_ID_CPE: {
                    r->state = MAAC_RAW_STATE_CPE;
                    maac_cpe_init(&r->ele.cpe);
                    break;
                }
                case MAAC_RAW_DATA_BLOCK_ID_CCE: {
                    return  MAAC_CCE_NOT_IMPLEMENTED;
                }
                case MAAC_RAW_DATA_BLOCK_ID_LFE: {
                    return  MAAC_LFE_NOT_IMPLEMENTED;
                }
                case MAAC_RAW_DATA_BLOCK_ID_DSE: {
                    return  MAAC_DSE_NOT_IMPLEMENTED;
                }
                case MAAC_RAW_DATA_BLOCK_ID_PCE: {
                    return  MAAC_PCE_NOT_IMPLEMENTED;
                }
                case MAAC_RAW_DATA_BLOCK_ID_FIL: {
                    r->state = MAAC_RAW_STATE_FIL;
                    maac_fil_init(&r->ele.fil);
                    break;
                }
                case MAAC_RAW_DATA_BLOCK_ID_END: {
                    maac_bitreader_byte_align(br);
                    break;
                }
            }
            return MAAC_OK;
        }
        default: break;
    }

    /* if we're here it means maac_raw_data_block_sync was called out-of-sequence */
    return MAAC_OUT_OF_SEQUENCE;
}

MAAC_PUBLIC
MAAC_RESULT
maac_raw_decode_fil(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br) {
    MAAC_RESULT res;

    if(r->state != MAAC_RAW_STATE_FIL) {
        return MAAC_OUT_OF_SEQUENCE;
    }
    if( (res = maac_fil_decode(&r->ele.fil, br)) != MAAC_OK) {
        return res;
    }
    r->state = MAAC_RAW_STATE_BLOCK_ID;
    return MAAC_OK;
}

MAAC_PUBLIC
MAAC_RESULT
maac_raw_decode_sce(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br, maac_channel* maac_restrict c) {
    MAAC_RESULT res;
    maac_sce_decode_params p;

    if(r->state != MAAC_RAW_STATE_SCE) {
        return MAAC_OUT_OF_SEQUENCE;
    }
    if(r->sf_index == 16) {
        return MAAC_SF_INDEX_NOT_SET;
    }

    p.sf_index = r->sf_index;
    p.ch = c;
    p.rand_state = &r->rand_state;

    if( (res = maac_sce_decode(&r->ele.sce, br, &p)) != MAAC_OK) {
        return res;
    }

    r->state = MAAC_RAW_STATE_BLOCK_ID;
    return MAAC_OK;
}

MAAC_PUBLIC
MAAC_RESULT
maac_raw_decode_cpe(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br, maac_channel* maac_restrict left, maac_channel* maac_restrict right) {
    MAAC_RESULT res;
    maac_cpe_decode_params p;

    if(r->state != MAAC_RAW_STATE_CPE) {
        return MAAC_OUT_OF_SEQUENCE;
    }
    if(r->sf_index == 16) {
        return MAAC_SF_INDEX_NOT_SET;
    }

    p.sf_index = r->sf_index;
    p.l = left;
    p.r = right;
    p.rand_state = &r->rand_state;

    if( (res = maac_cpe_decode(&r->ele.cpe, br, &p)) != MAAC_OK) {
        return res;
    }

    r->state = MAAC_RAW_STATE_BLOCK_ID;
    return MAAC_OK;
}

MAAC_PUBLIC
MAAC_RESULT
maac_raw_decode(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br) {
    MAAC_RESULT res;
    maac_channel* ch1;
    maac_channel* ch2;

    switch(r->state) {
        case MAAC_RAW_STATE_BLOCK_ID: {
            maac_raw_state_block_id:
            if( (res = maac_raw_sync(r, br)) != MAAC_OK) return res;
            r->_c = 0;
            switch(r->state) {
                case MAAC_RAW_STATE_BLOCK_ID: break;
                case MAAC_RAW_STATE_SCE: goto maac_raw_state_sce;
                case MAAC_RAW_STATE_CPE: goto maac_raw_state_cpe;
                case MAAC_RAW_STATE_FIL: goto maac_raw_state_fil;
                default: MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
            }
            return MAAC_OK;
        }

        case MAAC_RAW_STATE_SCE: {
            maac_raw_state_sce:
            ch1 = r->_c < r->num_out_channels ? &r->out_channels[r->_c] : NULL;
            if( (res = maac_raw_decode_sce(r, br, ch1)) != MAAC_OK) return res;
            r->_c++;
            goto maac_raw_state_block_id;
        }

        case MAAC_RAW_STATE_CPE: {
            maac_raw_state_cpe:
            ch1 = r->_c < r->num_out_channels ? &r->out_channels[r->_c] : NULL;
            ch2 = r->_c+1 < r->num_out_channels ? &r->out_channels[r->_c+1] : NULL;
            if( (res = maac_raw_decode_cpe(r, br, ch1, ch2)) != MAAC_OK) return res;
            r->_c += 2;
            goto maac_raw_state_block_id;
        }

        case MAAC_RAW_STATE_FIL: {
            maac_raw_state_fil:
            if( (res = maac_raw_decode_fil(r, br)) != MAAC_OK) return res;
            goto maac_raw_state_block_id;
        }

        default: break;
    }

    MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
}

