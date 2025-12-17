/* SPDX-License-Identifier: 0BSD */
#include "fil.h"

#include "bitreader_private.h"
#include "maac_memset.h"
#include "maac_unreachable.h"

MAAC_PUBLIC
void
maac_fil_init(maac_fil* f) {
    /* TODO - remove memset with garbage data once we verify this works correctly with mostly garbage data */
    maac_memset(f, 0xcc, sizeof *f);
    f->state = MAAC_FIL_STATE_COUNT;
}

MAAC_PUBLIC
MAAC_RESULT
maac_fil_decode(maac_fil* maac_restrict f, maac_bitreader* maac_restrict br) {
    MAAC_RESULT res;
    maac_u8 bits;

    switch(f->state) {
        case MAAC_FIL_STATE_COUNT: {
            if( (res = maac_bitreader_fill(br, 4)) != MAAC_OK) return res;
            f->_count = (maac_u16)maac_bitreader_read(br, 4);
            if(f->_count == 15) {
                f->state = MAAC_FIL_STATE_ESC_COUNT;
                goto maac_fil_state_esc_count;
            }
            f->state = MAAC_FIL_STATE_PAYLOAD_TYPE;
            goto maac_fil_state_payload_type;
        }

        case MAAC_FIL_STATE_ESC_COUNT: {
            maac_fil_state_esc_count:
            if( (res = maac_bitreader_fill(br, 8)) != MAAC_OK) return res;
            f->_count += (maac_u16)(maac_bitreader_read(br,8) - 1);
            f->state = MAAC_FIL_STATE_PAYLOAD_TYPE;
            goto maac_fil_state_payload_type;
        }

        case MAAC_FIL_STATE_PAYLOAD_TYPE: {
            maac_fil_state_payload_type:
            if(f->_count == 0) break;

            if( (res = maac_bitreader_fill(br, 4)) != MAAC_OK) return res;
            f->extension_type = maac_bitreader_read(br, 4);

            /* TODO SBR? */

            f->_bits = (8 * (f->_count-1)) + 4;
            f->state = MAAC_FIL_STATE_PAYLOAD_OTHER_BITS;
            goto maac_fil_state_payload_other_bits;
        }

        case MAAC_FIL_STATE_PAYLOAD_OTHER_BITS: {
            maac_fil_state_payload_other_bits:
            if(f->_bits == 0) break;

            bits = f->_bits > 32 ? 32 : f->_bits;
            if( (res = maac_bitreader_fill(br, bits)) != MAAC_OK) return res;
            maac_bitreader_discard(br, bits);
            f->_bits -= bits;
            goto maac_fil_state_payload_other_bits;
        }

        default: MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
    }

    f->state = MAAC_FIL_STATE_COUNT;
    return MAAC_OK;
}

