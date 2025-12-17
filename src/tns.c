/* SPDX-License-Identifier: 0BSD */
#include "tns.h"

#include "bitreader_private.h"
#include "maac_unreachable.h"
#include "maac_min.h"
#include "scalefactor_bands.h"
#include "maac_memset.h"
#include "tns_invquant_data.h"

static const maac_u8 TNS_MAX_BANDS_1024[16] = {
    31 /* 96 kHz */,
    31 /* 88.2 kHz */,
    34 /* 64 kHz */,
    40 /* 48 kHz */,
    42 /* 44.1 kHz */,
    51 /* 32 kHz */,
    46 /* 24 kHz */,
    46 /* 22.050 kHz */,
    42 /* 16 kHz */,
    42 /* 12 kHz */,
    42 /* 11.025 kHz */,
    39 /* 8 kHz */,
    0 /* reserved */,
    0 /* reserved */,
    0 /* reserved */,
    0 /* reserved */
};

static const maac_u8 TNS_MAX_BANDS_128[16] = {
    9 /* 96 kHz */,
    9 /* 88.2 kHz */,
    10 /* 64 kHz */,
    14 /* 48 kHz */,
    14 /* 44.1 kHz */,
    14 /* 32 kHz */,
    14 /* 24 kHz */,
    14 /* 22.050 kHz */,
    14 /* 16 kHz */,
    14 /* 12 kHz */,
    14 /* 11.025 kHz */,
    14 /* 8 kHz */,
    0 /* reserved */,
    0 /* reserved */,
    0 /* reserved */,
    0 /* reserved */
};

MAAC_PRIVATE
MAAC_RESULT
maac_tns_parse(maac_tns* maac_restrict tns, maac_bitreader* maac_restrict br, maac_u8 window_sequence) {
    const maac_u8 n_filt_bits = window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ? 1 : 2;
    const maac_u8 length_bits = window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ? 4 : 6;
    const maac_u8 order_bits  = window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ? 3 : 5;
    const maac_u8 num_windows = window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ? 8 : 1;

    maac_u8 coef_bits = 0;
    MAAC_RESULT res;

    switch(tns->state) {
        case MAAC_TNS_STATE_N_FILT: {
            maac_tns_data_state_n_filt:
            if( (res = maac_bitreader_fill(br, n_filt_bits)) != MAAC_OK) return res;
            tns->window[tns->_g].n_filt = (maac_u8)maac_bitreader_read(br, n_filt_bits);

            if(tns->window[tns->_g].n_filt) {
                tns->state = MAAC_TNS_STATE_COEF_RES;
                goto maac_ins_data_state_coef_res;
            }

            goto maac_tns_data_g_incr;
        }

        case MAAC_TNS_STATE_COEF_RES: {
            maac_ins_data_state_coef_res:
            if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
            tns->window[tns->_g].coef_res = (maac_u8) maac_bitreader_read(br, 1);
            tns->_k = 0;
            tns->state = MAAC_TNS_STATE_LENGTH;
            goto maac_tns_data_state_length;
        }

        case MAAC_TNS_STATE_LENGTH: {
            maac_tns_data_state_length:
            if( (res = maac_bitreader_fill(br, length_bits)) != MAAC_OK) return res;
            tns->window[tns->_g].filt[tns->_k].length = (maac_u8) maac_bitreader_read(br, length_bits);

            tns->state = MAAC_TNS_STATE_ORDER;
            goto maac_tns_data_state_order;
        }

        case MAAC_TNS_STATE_ORDER: {
            maac_tns_data_state_order:
            if( (res = maac_bitreader_fill(br, order_bits)) != MAAC_OK) return res;
            tns->window[tns->_g].filt[tns->_k].order = (maac_u8) maac_bitreader_read(br, order_bits);

            if(tns->window[tns->_g].filt[tns->_k].order) {
                tns->state = MAAC_TNS_STATE_DIRECTION;
                goto maac_tns_data_state_direction;
            }

            goto maac_tns_data_k_incr;
        }

        case MAAC_TNS_STATE_DIRECTION: {
            maac_tns_data_state_direction:
            if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
            tns->window[tns->_g].filt[tns->_k].direction = (maac_u8) maac_bitreader_read(br, 1);

            tns->state = MAAC_TNS_STATE_COEF_COMPRESS;
            goto maac_tns_data_state_coef_compress;
        }

        case MAAC_TNS_STATE_COEF_COMPRESS: {
            maac_tns_data_state_coef_compress:
            if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
            tns->window[tns->_g].filt[tns->_k].coef_compress = (maac_u8) maac_bitreader_read(br, 1);
            tns->_i = 0;
            tns->state = MAAC_TNS_STATE_COEF;
            goto maac_tns_data_state_coef;
        }

        case MAAC_TNS_STATE_COEF: {
            maac_tns_data_state_coef:
            coef_bits = 3 + tns->window[tns->_g].coef_res - tns->window[tns->_g].filt[tns->_k].coef_compress;
            if( (res = maac_bitreader_fill(br, coef_bits)) != MAAC_OK) return res;

            tns->window[tns->_g].filt[tns->_k].coef[tns->_i/2] &= 0xf0 >> (4 * (tns->_i % 2));
            tns->window[tns->_g].filt[tns->_k].coef[tns->_i/2] |=
              (maac_u8) maac_bitreader_read(br, coef_bits) << (4 * (tns->_i % 2));
            goto maac_tns_data_i_incr;
        }

        default: MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
    }

    maac_tns_data_i_incr:
    tns->_i++;
    if(tns->_i < tns->window[tns->_g].filt[tns->_k].order) {
        goto maac_tns_data_state_coef;
    }

    maac_tns_data_k_incr:
    tns->_k++;
    if(tns->_k < tns->window[tns->_g].n_filt) {
        tns->state = MAAC_TNS_STATE_LENGTH;
        goto maac_tns_data_state_length;
    }
    tns->state = MAAC_TNS_STATE_N_FILT;

    maac_tns_data_g_incr:
    tns->_g++;
    if(tns->_g == num_windows) {
        tns->_g = 0;
        return MAAC_OK;
    }
    goto maac_tns_data_state_n_filt;

    MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
}



static void maac_decode_coef(maac_u8 order, maac_u8 coef_res_bits, maac_u8 coef_compress, maac_u8* coef, maac_flt* a) {
    const maac_flt* tns_data = MAAC_TNS_INVQUANT[2 * coef_compress + coef_res_bits - 3];

    maac_u8 c;
    maac_u8 i;
    maac_u8 m;
    maac_u8 max;
    maac_flt tmp2[MAAC_TNS_TOTAL_ORDER];
    maac_flt b[MAAC_TNS_TOTAL_ORDER];

    for(i=0;i<order;i++) {
        c = (coef[i/2] >> (4 * (i % 2))) & 0x0f;
        tmp2[i] = tns_data[c];
    }

    a[0] = MAAC_FLT_C(1.0);
    for(m = 1; m <= order; m++) {
        for(i = 1; i < m; i++) {
            b[i] = a[i] + tmp2[m - 1] * a[m - i];
        }
        for(i = 1; i < m; i++) {
            a[i]  = b[i];
        }
        a[m] = tmp2[m - 1];
        if(m > max) max = m;
    }
}

static void
maac_tns_ar_filter(maac_flt* spec, maac_s16 size, maac_s16 inc, maac_flt* lpc, maac_u8 order) {
    maac_s16 i;
    maac_u8 j;
    maac_flt state[MAAC_TNS_MAX_ORDER];
    maac_flt val;

    for(j=0; j < order; j++) {
        state[j] = MAAC_FLT_C(0.0);
    }

    for(i=0; i < size; i++) {
        val = *spec;
        for(j=0;j<order;j++) {
            val -= lpc[j+1] * state[j];
        }
        for(j=order-1;j>0;j--) {
            state[j] = state[j-1];
        }
        state[0] = val;
        *spec = val;
        spec = &spec[inc];
    }
}

MAAC_PRIVATE
void
maac_tns_process(maac_tns* maac_restrict tns, maac_flt* maac_restrict samples, const maac_tns_params* maac_restrict p) {
    const maac_u8 num_windows = p->info->window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ? 8 : 1;

    const maac_u8 TNS_MAX_BANDS = p->info->window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ?
      TNS_MAX_BANDS_128[p->sf_index] : TNS_MAX_BANDS_1024[p->sf_index];

    const maac_u8 TNS_MAX_ORDER = p->info->window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ?
      MAAC_TNS_MAX_ORDER_128 : MAAC_TNS_MAX_ORDER_1024;

    const maac_scalefactor_bands b = maac_scalefactor_bandsf(p->info->window_sequence, p->sf_index);

    maac_u8 w;
    maac_u8 f;
    maac_u8 bottom;
    maac_u8 top;
    maac_u8 tns_order;

    maac_u16 start;
    maac_u16 end;
    maac_s16 size;
    maac_s16 inc;

    maac_flt lpc[MAAC_TNS_TOTAL_ORDER];

    maac_memset(lpc, 0, sizeof(lpc));

    for(w=0; w<num_windows; w++) {
        bottom = b.len;

        for(f=0; f<tns->window[w].n_filt; f++) {
            top = bottom;
            if(tns->window[w].filt[f].length <= top) {
                bottom = top - tns->window[w].filt[f].length;
            } else {
                bottom = 0;
            }
            tns_order = maac_min(tns->window[w].filt[f].order, TNS_MAX_ORDER);
            if(tns_order == 0) continue;

            maac_decode_coef(tns_order, 3 + tns->window[w].coef_res, tns->window[w].filt[f].coef_compress, tns->window[w].filt[f].coef, lpc);

            bottom = maac_min(bottom, TNS_MAX_BANDS);
            bottom = maac_min(bottom, p->info->max_sfb);

            top = maac_min(top, TNS_MAX_BANDS);
            top = maac_min(top, p->info->max_sfb);

            start = b.offsets[bottom];
            end   = b.offsets[top];
            if(start >= end) continue;

            size = (maac_s16)( end - start );

            if(tns->window[w].filt[f].direction) {
                inc = -1;
                start = end - 1;
            } else {
                inc = 1;
            }
            maac_tns_ar_filter(&samples[(w * 128) + start], size, inc, lpc, tns_order);
        }
    }

    return;
}
