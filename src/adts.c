/* SPDX-License-Identifier: 0BSD */
#include "adts.h"

#include "maac_memset.h"
#include "maac_unreachable.h"
#include "bitreader_private.h"
#include "maac_frequency.h"

MAAC_PUBLIC
void
maac_adts_init(maac_adts* a) {
    a->state = MAAC_ADTS_STATE_SYNCWORD;
    maac_raw_init(&a->raw);
    /* maac_memset(a, 0, sizeof *a); */
}

MAAC_PUBLIC
MAAC_RESULT
maac_adts_sync(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br) {
    MAAC_RESULT res;
    maac_u32 tmp;

    switch(a->state) {
        case MAAC_ADTS_STATE_SYNCWORD: {
            maac_adts_state_syncword:
            if( (res = maac_bitreader_fill(br,12)) != MAAC_OK) return res;
            tmp = (maac_u32)maac_bitreader_peek(br,12);
            if(tmp == 0x0fff) {
                maac_bitreader_discard(br,12);
                a->_i = 0;
                a->state = MAAC_ADTS_STATE_VERSION;
                maac_memset(&a->fixed_header, 0, sizeof(a->fixed_header));
                maac_memset(&a->variable_header, 0, sizeof(a->variable_header));
                goto maac_adts_state_version;
            }
                
            if(a->_i == a->tolerance) {
                return MAAC_ADTS_SYNCWORD_NOT_FOUND;
            }

            maac_bitreader_discard(br,8);
            a->_i++;
            goto maac_adts_state_syncword;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_VERSION: {
            maac_adts_state_version:
            if((res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            a->fixed_header.version = maac_bitreader_read(br,1);
            a->state = MAAC_ADTS_STATE_LAYER;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_LAYER: {
            if((res = maac_bitreader_fill(br,2)) != MAAC_OK) return res;
            a->fixed_header.layer = maac_bitreader_read(br,2);
            a->state = MAAC_ADTS_STATE_PROTECTION;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_PROTECTION: {
            if((res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            a->fixed_header.protection_absent = maac_bitreader_read(br,1);
            a->state = MAAC_ADTS_STATE_PROFILE;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_PROFILE: {
            if((res = maac_bitreader_fill(br,2)) != MAAC_OK) return res;
            a->fixed_header.profile = maac_bitreader_read(br,2);
            a->state = MAAC_ADTS_STATE_FREQ_INDEX;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_FREQ_INDEX: {
            if((res = maac_bitreader_fill(br,4)) != MAAC_OK) return res;
            a->fixed_header.sampling_frequency_index = maac_bitreader_read(br,4);
            a->state = MAAC_ADTS_STATE_PRIVATE;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_PRIVATE: {
            if((res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            maac_bitreader_discard(br,1);
            a->state = MAAC_ADTS_STATE_CHANNEL_INDEX;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_CHANNEL_INDEX: {
            if((res = maac_bitreader_fill(br,3)) != MAAC_OK) return res;
            a->fixed_header.channel_configuration = maac_bitreader_read(br,3);
            a->state = MAAC_ADTS_STATE_ORIG;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_ORIG: {
            if((res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            a->fixed_header.original_copy = maac_bitreader_read(br,1);
            a->state = MAAC_ADTS_STATE_HOME;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_HOME: {
            if((res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            a->fixed_header.home = maac_bitreader_read(br,1);
            a->state = MAAC_ADTS_STATE_COPYRIGHT;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_COPYRIGHT: {
            if((res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            a->variable_header.copyright_id_bit = maac_bitreader_read(br,1);
            a->state = MAAC_ADTS_STATE_COPYRIGHT_START;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_COPYRIGHT_START: {
            if((res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            a->variable_header.copyright_id_start = maac_bitreader_read(br,1);
            a->state = MAAC_ADTS_STATE_LENGTH;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_LENGTH: {
            if((res = maac_bitreader_fill(br,13)) != MAAC_OK) return res;
            a->variable_header.frame_length = maac_bitreader_read(br,13);
            a->state = MAAC_ADTS_STATE_BUFFER_FULLNESS;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_BUFFER_FULLNESS: {
            if((res = maac_bitreader_fill(br,11)) != MAAC_OK) return res;
            a->variable_header.buffer_fullness = maac_bitreader_read(br,11);
            a->state = MAAC_ADTS_STATE_RAW_DATA_BLOCKS;
        }
        /* fall-through */
        case MAAC_ADTS_STATE_RAW_DATA_BLOCKS: {
            if((res = maac_bitreader_fill(br,2)) != MAAC_OK) return res;
            a->variable_header.raw_data_blocks = maac_bitreader_read(br,2);

            /* if the protection_absent flag is set - there's no CRC
            data so we're ready to roll */
            if(a->fixed_header.protection_absent) {
                a->state = MAAC_ADTS_STATE_RAW_DATA_BLOCK;
                a->_i = 0;
                /* configure the raw data block */
                a->raw.sf_index = a->fixed_header.sampling_frequency_index;
                a->raw.sample_rate = maac_sampling_frequency(a->raw.sf_index);
                return MAAC_OK;
            }

            if(a->variable_header.raw_data_blocks == 0) {
                a->state = MAAC_ADTS_STATE_CRC16;
                goto maac_adts_state_crc16;
            }
            /* we have multiple data blocks, and positions to read */
            a->state = MAAC_ADTS_STATE_RAW_DATA_BLOCK_POSITION;
            a->_i = 0;
            goto maac_adts_state_raw_data_block_position;
        }

        case MAAC_ADTS_STATE_RAW_DATA_BLOCK_POSITION: {
            maac_adts_state_raw_data_block_position:
            while(a->_i < a->variable_header.raw_data_blocks) {
                if( (res = maac_bitreader_fill(br,16)) != MAAC_OK) return res;
                a->raw_data_block_position[a->_i] = maac_bitreader_read(br,16);
                a->_i++;
            }
            a->state = MAAC_ADTS_STATE_CRC16;
            goto maac_adts_state_crc16;
        }

        case MAAC_ADTS_STATE_CRC16: {
            maac_adts_state_crc16:
            if((res = maac_bitreader_fill(br,16)) != MAAC_OK) return res;
            a->crc = (maac_u16)maac_bitreader_read(br,16);

            a->state = MAAC_ADTS_STATE_RAW_DATA_BLOCK;
            a->_i = 0;
            a->raw.sf_index = a->fixed_header.sampling_frequency_index;
            a->raw.sample_rate = maac_sampling_frequency(a->raw.sf_index);
            return MAAC_OK;
        }

        case MAAC_ADTS_STATE_RAW_DATA_BLOCK: {
            return MAAC_OUT_OF_SEQUENCE;
#if 0
            if(a->_i <= a->variable_header.raw_data_blocks) {
                /* this means maac_adts_rdb wasn't called to count off a
                   raw data block */
                return MAAC_ADTS_RDB_NOT_CALLED;
            }
            break;
#endif
        }
        case MAAC_ADTS_STATE_RAW_DATA_BLOCK_CRC16: {
            return MAAC_OUT_OF_SEQUENCE;
        }
    }

    MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
}

MAAC_PUBLIC
MAAC_RESULT
maac_adts_raw_sync(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br) {
    MAAC_RESULT res;

    if(a->state == MAAC_ADTS_STATE_RAW_DATA_BLOCK_CRC16) {
        maac_adts_raw_sync_crc16:
        if( (res = maac_bitreader_fill(br, 16)) != MAAC_OK) return res;
        maac_bitreader_discard(br, 16);
        a->state = MAAC_ADTS_STATE_RAW_DATA_BLOCK;
        return MAAC_OK;
    }

    if(a->state != MAAC_ADTS_STATE_RAW_DATA_BLOCK) {
        return MAAC_OUT_OF_SEQUENCE;
    }

    if( (res = maac_raw_sync(&a->raw, br)) != MAAC_OK) {
        return res;
    }

    if(a->raw.ele_id == MAAC_RAW_DATA_BLOCK_ID_END) {
        /* if we have to read a trailing CRC16 we don't
        want to return until we've read it */
        if(a->variable_header.raw_data_blocks && !(a->fixed_header.protection_absent)) {
            a->state = MAAC_ADTS_STATE_RAW_DATA_BLOCK_CRC16;
            goto maac_adts_raw_sync_crc16;
        }
    }

    return MAAC_OK;
}

MAAC_PUBLIC
MAAC_RESULT
maac_adts_raw_decode_fil(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br) {
    if(a->state != MAAC_ADTS_STATE_RAW_DATA_BLOCK) {
        return MAAC_OUT_OF_SEQUENCE;
    }
    return maac_raw_decode_fil(&a->raw, br);
}

MAAC_PUBLIC
MAAC_RESULT
maac_adts_raw_decode_sce(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br, maac_channel* maac_restrict c) {
    if(a->state != MAAC_ADTS_STATE_RAW_DATA_BLOCK) {
        return MAAC_OUT_OF_SEQUENCE;
    }
    return maac_raw_decode_sce(&a->raw, br, c);
}

MAAC_PUBLIC
MAAC_RESULT
maac_adts_raw_decode_cpe(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br, maac_channel* maac_restrict left, maac_channel* maac_restrict right) {
    if(a->state != MAAC_ADTS_STATE_RAW_DATA_BLOCK) {
        return MAAC_OUT_OF_SEQUENCE;
    }
    return maac_raw_decode_cpe(&a->raw, br, left, right);
}

MAAC_PUBLIC
MAAC_RESULT
maac_adts_decode(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br) {
    MAAC_RESULT res;

    switch(a->state) {
        case MAAC_ADTS_STATE_RAW_DATA_BLOCK: {
            maac_adts_decode_raw_data_block:
            if( (res = maac_raw_decode(&a->raw, br)) !=  MAAC_OK) return res;
            /* maac_raw_decode only returns OK after decoding the END element, so
            check if we have a CRC16 value to read */
            if(a->variable_header.raw_data_blocks && !(a->fixed_header.protection_absent)) {
                a->state = MAAC_ADTS_STATE_RAW_DATA_BLOCK_CRC16;
                goto maac_adts_decode_crc16;
            }
            break;
        }
        case MAAC_ADTS_STATE_RAW_DATA_BLOCK_CRC16: {
            maac_adts_decode_crc16:
            if( (res = maac_bitreader_fill(br, 16)) != MAAC_OK) return res;
            maac_bitreader_discard(br, 16);
            break;
        }
        default: {
            if( (res = maac_adts_sync(a, br)) != MAAC_OK) return res;
            goto maac_adts_decode_raw_data_block;
        }
    }

    /* either way we're at the end of a raw data block so we'll return MAAC_OK, but
    if we have more blocks to read we'll go back to reading, else go back to looking
    for the syncword */
    a->_i++;
    if(a->_i <= a->variable_header.raw_data_blocks) {
        a->state = MAAC_ADTS_STATE_RAW_DATA_BLOCK;
    } else {
        a->state = MAAC_ADTS_STATE_SYNCWORD;
        a->_i = 0;
    }
    return MAAC_OK;
}

