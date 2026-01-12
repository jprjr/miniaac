/* SPDX-License-Identifier: 0BSD */
#include "cpe.h"
#include "tns.h"
#include "pns.h"
#include "bitreader_private.h"

#include "maac_memset.h"
#include "maac_memcpy.h"
#include "maac_unreachable.h"
#include "maac_clamp.h"
#include "maac_consts.h"
#include "maac_math.h"
#include "maac_rand.h"

#include "window_group_lengths.h"
#include "scalefactor_bands.h"
#include "sfg_num_window_groups.h"
#include "maac_section_idx.h"

#include "filterbank.h"

#define maac_cpe_section_idx(g,i) ( ((MAAC_MAX_SWB_OFFSET_SHORT_WINDOW) * (g)) + (i))
#define maac_ms_used_idx(g,i) ( ((MAAC_MAX_SWB_OFFSET_SHORT_WINDOW) * (g)) + (i))

MAAC_PUBLIC
void
maac_cpe_init(maac_cpe* c) {
    /* TODO - remove memset with garbage data once we verify this works correctly with mostly garbage data */
    maac_memset(c, 0xdf, sizeof *c);
    c->state = MAAC_CPE_STATE_TAG;
}

static void maac_cpe_ms(maac_cpe* maac_restrict c, maac_flt* maac_restrict l_samples, maac_flt* maac_restrict r_samples, maac_u8 sf_index) {
    const maac_u8 num_groups = maac_sfg_num_window_groups(c->info.scale_factor_grouping);
    const maac_u8 max_sfb = c->info.max_sfb;
    const maac_scalefactor_bands b = maac_scalefactor_bandsf(c->info.window_sequence, sf_index);

    maac_u32 group_lengths = maac_window_group_lengths(c->info.window_sequence, c->info.scale_factor_grouping);

    maac_u8 cb_l;
    maac_u8 cb_r;
    maac_u8 g;
    maac_u8 w;
    maac_u8 sfb;
    maac_u32 idx;
    maac_u8 section_idx_l;
    maac_u8 section_idx_r;
    maac_u8 group_len;
    maac_u16 i;
    maac_u16 group_off;
    maac_u16 off;
    maac_flt tmp;

    off = 0;
    group_off = 0;

    for(g = 0; g < num_groups; g++) {
        group_len = group_lengths & 0x0f;
        section_idx_l = 0;
        section_idx_r = 0;
        sfb = 0;

        while(sfb < max_sfb) {
            cb_l = c->ics_l.section_data[maac_cpe_section_idx(g, section_idx_l)].codebook;
            cb_r = c->ics_r.section_data[maac_cpe_section_idx(g, section_idx_r)].codebook;

            idx = maac_cpe_section_idx(g, sfb);

            if(cb_r < MAAC_NOISE_HCB && cb_l < MAAC_NOISE_HCB && c->ms_mask_present && ((c->ms_used[idx/32] >> (idx % 32)) & 0x01)) {
                for(w=0 ; w < group_len ; w++) {
                    off = group_off + (w * 128);
                    for(i=b.offsets[sfb];i<b.offsets[sfb+1];i++) {
                        tmp = l_samples[off + i] - r_samples[off + i];
                        l_samples[off + i] = l_samples[off + i] + r_samples[off + i];
                        r_samples[off + i] = tmp;
                    }

                }
            }
            sfb++;
            if(sfb == c->ics_l.section_data[maac_cpe_section_idx(g, section_idx_l)].end) {
                section_idx_l++;
            }
            if(sfb == c->ics_r.section_data[maac_cpe_section_idx(g, section_idx_r)].end) {
                section_idx_r++;
            }
        }

        group_off += group_len * 128;
        group_lengths >>= 4;
    }
}

/* we specifically want to use ics_r's info since intensity stereo can be used with
or without a common window */
static void maac_cpe_is(maac_cpe* maac_restrict c, maac_flt* maac_restrict l_samples, maac_flt* maac_restrict r_samples, maac_u8 sf_index) {
    const maac_u8 num_groups = maac_sfg_num_window_groups(c->ics_r.info.scale_factor_grouping);
    const maac_u8 max_sfb = c->ics_r.info.max_sfb;
    const maac_scalefactor_bands b = maac_scalefactor_bandsf(c->ics_r.info.window_sequence, sf_index);

    maac_u32 group_lengths = maac_window_group_lengths(c->ics_r.info.window_sequence, c->ics_r.info.scale_factor_grouping);

    maac_u8 cb_r;
    maac_u8 g;
    maac_u8 w;
    maac_u8 sfb;
    maac_u32 idx;
    maac_u8 section_idx_r;
    maac_u8 group_len;
    maac_u16 i;
    maac_u16 group_off;
    maac_u16 off;
    maac_flt tmp;

    off = 0;
    group_off = 0;

    for(g = 0; g < num_groups; g++) {
        group_len = group_lengths & 0x0f;
        section_idx_r = 0;
        sfb = 0;

        while(sfb < max_sfb) {
            cb_r = c->ics_r.section_data[maac_cpe_section_idx(g, section_idx_r)].codebook;

            idx = maac_cpe_section_idx(g, sfb);

            if((cb_r == MAAC_INTENSITY_HCB || cb_r == MAAC_INTENSITY_HCB2)) {
                /* negating the value lets us do pow2(-x/4) instead of pow(0.5,x/4) */
                tmp = maac_pow2_xdiv4(-c->ics_r.scalefactors[maac_cpe_section_idx(g,sfb)]);

                if(cb_r == MAAC_INTENSITY_HCB2) tmp *= -1;
                if(c->ms_mask_present == 1) {
                    tmp *= (((c->ms_used[idx/32] >> (idx % 32)) & 0x01) ? MAAC_FLT_C(-1.0) : MAAC_FLT_C(1.0));
                }

                for(w=0 ; w < group_len ; w++) {
                    off = group_off + (w * 128);
                    for(i=b.offsets[sfb];i<b.offsets[sfb+1];i++) {
                        r_samples[off + i] = l_samples[off + i] * tmp;
                    }

                }
            }
            sfb++;
            if(sfb == c->ics_r.section_data[maac_cpe_section_idx(g, section_idx_r)].end) {
                section_idx_r++;
            }
        }

        group_off += group_len * 128;
        group_lengths >>= 4;
    }
}

static
MAAC_RESULT
maac_cpe_decode_mask_used(maac_cpe* maac_restrict c, maac_bitreader* maac_restrict br) {
    MAAC_RESULT res;
    maac_u32 i;
    const maac_u8 num_groups = (maac_u8)maac_sfg_num_window_groups(c->info.scale_factor_grouping);

    while(c->_g < num_groups) {
        while(c->_sfb < c->info.max_sfb) {

            if( (res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;

            i = maac_ms_used_idx( ((maac_u32)c->_g), ((maac_u32)c->_sfb) );
            c->ms_used[i/32] |= maac_bitreader_read(br, 1) << (i % 32);
            c->_sfb++;

        }
        c->_sfb = 0;
        c->_g++;
    }
    return MAAC_OK;
}


MAAC_PUBLIC
MAAC_RESULT
maac_cpe_decode(maac_cpe* maac_restrict c, maac_bitreader* maac_restrict br, const maac_cpe_decode_params* maac_restrict p) {
    MAAC_RESULT res;
    maac_ics_decode_params ics_p;
    maac_filterbank_params fb_p;
    maac_tns_params tns_p;
    maac_pns_params pns_p;

    ics_p.common_window = 0;
    ics_p.sf_index = p->sf_index;
    ics_p.ch = NULL;

    pns_p.sf_index = p->sf_index;
    pns_p.rand_state = p->rand_state;

    switch(c->state) {

        case MAAC_CPE_STATE_TAG: {
            if( (res = maac_bitreader_fill(br, 4)) != MAAC_OK) return res;
            c->element_instance_tag = (maac_u8)maac_bitreader_read(br, 4);

            c->state = MAAC_CPE_STATE_COMMON_WINDOW;
            goto maac_cpe_state_common_window;
        }

        case MAAC_CPE_STATE_COMMON_WINDOW: {
            maac_cpe_state_common_window:
            if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
            c->common_window = maac_bitreader_read(br, 1);

            /* blank out mid-side to fill in (or not fill in) later */
            c->ms_used[0] = 0;
            c->ms_used[1] = 0;
            c->ms_used[2] = 0;
            c->ms_used[3] = 0;

            if(c->common_window) {
                c->state = MAAC_CPE_STATE_ICS_INFO;
                maac_ics_info_init(&c->info);
                goto maac_cpe_state_ics_info;
            }

            c->state = MAAC_CPE_STATE_ICS_LEFT;
            maac_ics_init(&c->ics_l);
            goto maac_cpe_state_ics_left;
        }

        case MAAC_CPE_STATE_ICS_INFO: {
            maac_cpe_state_ics_info:
            if( (res = maac_ics_info_parse(&c->info, br, p->sf_index)) != MAAC_OK) return res;
            c->state = MAAC_CPE_STATE_MS_MASK_PRESENT;
            goto maac_cpe_state_ms_mask_present;
        }

        case MAAC_CPE_STATE_MS_MASK_PRESENT: {
            maac_cpe_state_ms_mask_present:
            if( (res = maac_bitreader_fill(br, 2)) != MAAC_OK) return res;
            c->ms_mask_present = maac_bitreader_read(br, 2);
            switch(c->ms_mask_present) {
                case 0x01: {
                    c->state = MAAC_CPE_STATE_MS_USED;
                    c->_g = 0;
                    c->_sfb = 0;
                    c->ms_used[0] = 0;
                    c->ms_used[1] = 0;
                    c->ms_used[2] = 0;
                    c->ms_used[3] = 0;
                    goto maac_cpe_state_ms_used;
                }
                case 0x02: {
                    c->ms_used[0] = ~0;
                    c->ms_used[1] = ~0;
                    c->ms_used[2] = ~0;
                    c->ms_used[3] = ~0;
                }
                /* fall-through */
                default: break;
            }

            c->state = MAAC_CPE_STATE_ICS_LEFT;
            maac_ics_init(&c->ics_l);
            maac_memcpy(&c->ics_l.info, &c->info, sizeof c->info);
            goto maac_cpe_state_ics_left;
        }

        case MAAC_CPE_STATE_MS_USED: {
            maac_cpe_state_ms_used:
            if( (res = maac_cpe_decode_mask_used(c,br)) != MAAC_OK) return res;

            c->state = MAAC_CPE_STATE_ICS_LEFT;
            maac_ics_init(&c->ics_l);
            maac_memcpy(&c->ics_l.info, &c->info, sizeof c->info);
            goto maac_cpe_state_ics_left;
        }


        case MAAC_CPE_STATE_ICS_LEFT: {
            maac_cpe_state_ics_left:
            ics_p.common_window = c->common_window;
            ics_p.ch = p->l;
            if( (res = maac_ics_decode(&c->ics_l, br, &ics_p)) != MAAC_OK) return res;
            c->state = MAAC_CPE_STATE_ICS_RIGHT;
            maac_ics_init(&c->ics_r);
            if(c->common_window) {
                maac_memcpy(&c->ics_r.info, &c->info, sizeof c->info);
            }
            goto maac_cpe_state_ics_right;
        }

        case MAAC_CPE_STATE_ICS_RIGHT: {
            maac_cpe_state_ics_right:
            ics_p.common_window = c->common_window;
            ics_p.ch = p->r;
            if( (res = maac_ics_decode(&c->ics_r, br, &ics_p)) != MAAC_OK) return res;
            c->state = MAAC_CPE_STATE_TAG;
            break;
        }

        default: MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
    }

    /* the high-level order of a decoder is:
      1. huffman decoding
      2. inverse quantization
      3. rescaling
      - that's all handled in ics
      4. mid-side
      5. prediction (not used in LC)
      6. intensity stereo - done at same time as mid-side
      7. dependently switched coupling (??)
      8. TNS
      9. dependently switched coupling (??)
      10. filterbank
      11. gain control (not used in LC)
      12. independently switched coupling (I don't know what this is?)
    */

    /* perceptual noise is really kind of partof inverse quantization + rescaling,
    but we want to maintain random state between channels so we do it here,
    plus if there's mid-side flags on a band you're supposed to use the same
    state for both channels (or what I'll probably do is just copy samples over,
    same thing.)

    I don't know if any encoders actually do all of that though? */

    if(! (p->l == NULL || p->r == NULL) ) {
        pns_p.spectra = p->l->samples;
        pns_p.ics = &c->ics_l;
        maac_pns_process(&pns_p);

        pns_p.spectra = p->r->samples;
        pns_p.ics = &c->ics_r;
        maac_pns_process(&pns_p);

        maac_cpe_ms(c, p->l->samples, p->r->samples, p->sf_index);

        maac_cpe_is(c, p->l->samples, p->r->samples, p->sf_index);

        if(c->ics_l.tns_data_present) {
            tns_p.sf_index = p->sf_index;
            tns_p.info = &c->ics_l.info;
            maac_tns_process(&c->ics_l.tns, &p->l->samples[0], &tns_p);
        }

        if(c->ics_r.tns_data_present) {
            tns_p.sf_index = p->sf_index;
            tns_p.info = (c->common_window ? &c->ics_l.info : &c->ics_r.info);
            maac_tns_process(&c->ics_r.tns, &p->r->samples[0], &tns_p);
        }

        fb_p.window_sequence   = c->ics_l.info.window_sequence;
        fb_p.window_shape      = c->ics_l.info.window_shape;
        fb_p.window_shape_prev = p->l->window_shape_prev;
        maac_filterbank(&p->l->samples[0], &p->l->samples[2048], &fb_p);
        p->l->window_shape_prev = fb_p.window_shape;

        if(!c->common_window) {
            fb_p.window_sequence   = c->ics_r.info.window_sequence;
            fb_p.window_shape      = c->ics_r.info.window_shape;
        }
        fb_p.window_shape_prev = p->r->window_shape_prev;
        maac_filterbank(&p->r->samples[0], &p->r->samples[2048], &fb_p);
        p->r->window_shape_prev = fb_p.window_shape;

        p->l->n_samples = 1024;
        p->l->_n = 0;

        p->r->n_samples = 1024;
        p->r->_n = 0;
    }

    return MAAC_OK;
}
