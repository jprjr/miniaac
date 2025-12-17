/* SPDX-License-Identifier: 0BSD */
#include "sce.h"
#include "tns.h"
#include "filterbank.h"
#include "pns.h"

#include "bitreader_private.h"
#include "maac_memset.h"
#include "maac_unreachable.h"
#include "maac_clamp.h"


MAAC_PUBLIC
void
maac_sce_init(maac_sce* s) {
    /* TODO - remove memset with garbage data once we verify this works correctly with mostly garbage data */
    maac_memset(s, 0xab, sizeof *s);
    s->state = MAAC_SCE_STATE_TAG;
}

MAAC_PUBLIC
MAAC_RESULT
maac_sce_decode(maac_sce* maac_restrict s, maac_bitreader* maac_restrict br, const maac_sce_decode_params* maac_restrict p) {
    MAAC_RESULT res;
    maac_ics_decode_params ics_p;
    maac_filterbank_params fb_p;
    maac_tns_params tns_p;
    maac_pns_params pns_p;

    ics_p.common_window = 0;
    ics_p.sf_index = p->sf_index;
    ics_p.ch = p->ch;

    switch(s->state) {

        case MAAC_SCE_STATE_TAG: {
            if( (res = maac_bitreader_fill(br, 4)) != MAAC_OK) return res;
            s->state = MAAC_SCE_STATE_ICS;

            s->element_instance_tag = (maac_u8)maac_bitreader_read(br, 4);
            maac_ics_init(&s->ics);
            goto maac_sce_state_ics;
        }

        case MAAC_SCE_STATE_ICS: {
            maac_sce_state_ics:
            if( (res = maac_ics_decode(&s->ics, br, &ics_p)) != MAAC_OK) return res;
            s->state = MAAC_SCE_STATE_TAG;
            break;
        }

        default: MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
    }

    if(p->ch != NULL) {
        pns_p.rand_state = p->rand_state;
        pns_p.sf_index = p->sf_index;
        pns_p.spectra = p->ch->samples;
        pns_p.ics = &s->ics;

        maac_pns_process(&pns_p);

        if(s->ics.tns_data_present) {
            tns_p.sf_index = p->sf_index;
            tns_p.info = &s->ics.info;
            maac_tns_process(&s->ics.tns, &p->ch->samples[0], &tns_p);
        }

        fb_p.window_sequence   = s->ics.info.window_sequence;
        fb_p.window_shape      = s->ics.info.window_shape;
        fb_p.window_shape_prev = p->ch->window_shape_prev;
        maac_filterbank(&p->ch->samples[0], &p->ch->samples[2048], &fb_p);
        p->ch->window_shape_prev = fb_p.window_shape;

        p->ch->n_samples = 1024;
        p->ch->_n = 0;
    }

    return MAAC_OK;
}
