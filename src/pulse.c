/* SPDX-License-Identifier: 0BSD */
#include "pulse.h"

#include "bitreader_private.h"
#include "maac_unreachable.h"

MAAC_PRIVATE
void
maac_pulse_init(maac_pulse* p) {
    p->state = MAAC_PULSE_STATE_NUMBER;
}

MAAC_PRIVATE
MAAC_RESULT
maac_pulse_parse(maac_pulse* maac_restrict p, maac_bitreader* maac_restrict br) {
    MAAC_RESULT res;

    switch(p->state) {
        case MAAC_PULSE_STATE_NUMBER: {
            if( (res = maac_bitreader_fill(br, 2)) != MAAC_OK) return res;
            p->num_pulse = maac_bitreader_read(br,2) + 1;
            p->state = MAAC_PULSE_STATE_START_SFB;
            goto maac_pulse_state_start_sfb;
        }

        case MAAC_PULSE_STATE_START_SFB: {
            maac_pulse_state_start_sfb:
            if( (res = maac_bitreader_fill(br, 6)) != MAAC_OK) return res;
            p->start_sfb = maac_bitreader_read(br, 6);
            p->_n = 0;
            p->state = MAAC_PULSE_STATE_OFFSET;
            goto maac_pulse_state_offset;
        }

        case MAAC_PULSE_STATE_OFFSET: {
            maac_pulse_state_offset:
            if( (res = maac_bitreader_fill(br, 5)) != MAAC_OK) return res;
            p->pulses[p->_n] = maac_bitreader_read(br,5);
            p->state = MAAC_PULSE_STATE_AMP;
            goto maac_pulse_state_amp;
        }

        case MAAC_PULSE_STATE_AMP: {
            maac_pulse_state_amp:
            if( (res = maac_bitreader_fill(br, 4)) != MAAC_OK) return res;
            p->pulses[p->_n++] |= maac_bitreader_read(br, 4) << 5;
            if(p->_n == p->num_pulse) break;

            p->state = MAAC_PULSE_STATE_OFFSET;
            goto maac_pulse_state_offset;
        }

        default: MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
    }

    p->state = MAAC_PULSE_STATE_NUMBER;
    return MAAC_OK;
}
