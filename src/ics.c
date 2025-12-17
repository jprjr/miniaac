/* SPDX-License-Identifier: 0BSD */
#include "ics.h"

#include "bitreader_private.h"
#include "maac_assert.h"
#include "maac_memset.h"
#include "maac_inline.h"
#include "maac_unreachable.h"
#include "maac_consts.h"
#include "maac_math.h"
#include "maac_abs.h"
#include "maac_clamp.h"
#include "scalefactor_bands.h"
#include "window_group_lengths.h"
#include "sfg_num_window_groups.h"
#include "maac_section_idx.h"

/* you can define MAAC_INVQUANT_TABLES to use lookup tables
for inverse quantization. I'm not sure if I recommend this -
it adds about 32kB of const data to the build. */
#ifdef MAAC_INVQUANT_TABLES
#include "inv_quant_tables.h"
#endif

MAAC_PUBLIC
void
maac_ics_init(maac_ics* ics) {
    /* TODO - remove memset with garbage data once we verify this works correctly with mostly garbage data */
    maac_memset(ics, 0xCD, sizeof *ics);
    ics->state = MAAC_ICS_STATE_GLOBAL_GAIN;
}

static maac_inline void
maac_ics_reset_iterators(maac_ics* ics) {
    ics->_g = 0;
    ics->_w = 0;
    ics->_i = 0;
    ics->_k = 0;
    ics->_p = 0;
    ics->_off = 0;
    ics->_group_off = 0;
}

static maac_inline void
maac_ics_reset_scalefactors(maac_ics* ics) {
    ics->_noise_flag = 1;
    ics->_scale_factor = ics->global_gain;
    ics->_dpcm_is_position = 0;
    ics->_noise_energy = ((maac_s32)ics->global_gain) - 90;
}

struct maac_ics_codebook_params {
    maac_u8 cb;
    maac_u8 group_len;
    maac_u16 sfb_start;
    maac_u16 sfb_end;
    maac_flt* samples;
};

typedef struct maac_ics_codebook_params maac_ics_codebook_params;

static void
maac_ics_zero_codebook(maac_ics* ics, const maac_ics_codebook_params* p) {
    maac_u8 group;
    maac_u16 sfb;
    maac_u16 off;

    for(group=0; group < p->group_len; group++) {

        off = ics->_group_off + (group * 128);

        for(sfb=p->sfb_start; sfb < p->sfb_end; sfb++) {
            p->samples[off + sfb] = 0;
        }
    }
}

static MAAC_RESULT
maac_ics_default_codebook(maac_ics* ics, maac_bitreader* maac_restrict br, const maac_ics_codebook_params* p) {
    MAAC_RESULT res;
    maac_u8 n;
    maac_u8 num;
    maac_u16 off;
    maac_s16 sf;

    maac_flt scale;
    maac_flt quant;

    num = p->cb >= MAAC_FIRST_PAIR_HCB ? 2 : 4;

    sf = ics->scalefactors[ maac_section_idx(ics->_g, ics->_k) ];
    /* this one I'm more certain the range is 0 - 255 */
    maac_assert(sf >= 0);
    maac_assert(sf <= 255);
    sf = maac_clamp(sf, 0, 255);

    scale = maac_pow2_xdiv4(sf - 100);

    while(ics->_w < p->group_len) {
        off = ics->_group_off + (ics->_w * 128);

        while(p->sfb_start + ics->_p < p->sfb_end) {
            if( (res = maac_huffman_decode_spectral(&ics->_huffman, br, p->cb, ics->spectra_tmp)) != MAAC_OK) return res;

            if(p->samples != NULL) {
                for(n=0;n<num;n++) {
#ifdef MAAC_INVQUANT_TABLES
                    quant = (ics->spectra_tmp[n] < 0 ?
                       -MAAC_INV_QUANT[-ics->spectra_tmp[n]]
                       :
                       MAAC_INV_QUANT[ics->spectra_tmp[n]]);
#else
                    quant =
                        maac_flt_cast(ics->spectra_tmp[n]) *
                        maac_cbrt(maac_abs(ics->spectra_tmp[n]));
#endif
                    quant *= scale;
                    p->samples[off + p->sfb_start + ics->_p + n] = quant;
                }
            }
            ics->_p += num;
        }
        ics->_p = 0;
        ics->_w++;
    }
    ics->_w = 0;
    return MAAC_OK;
}


static MAAC_RESULT
maac_spectra_decode(maac_ics* ics, maac_bitreader* maac_restrict br, const maac_ics_decode_params* p) {
    const maac_ics_info* info = &ics->info;
    const maac_u8 num_window_groups = maac_sfg_num_window_groups(info->scale_factor_grouping);
    const maac_scalefactor_bands b = maac_scalefactor_bandsf(info->window_sequence, p->sf_index);

    maac_u32 group_lengths = maac_window_group_lengths(info->window_sequence, info->scale_factor_grouping) >> (ics->_g * 4);

    MAAC_RESULT res;

    maac_u32 idx;

    maac_ics_codebook_params cb_p;
    cb_p.samples = p->ch == NULL ? NULL : p->ch->samples;

    /* g tracks window groups, easy enough */
    /* i tracks the section within a group */
    /* k tracks the scale factor band *index* over the section (0-51 / 0-15) */
    /* p tracks the scale factor *band* over the section,
       relative to the start value, so it goes from
       0 -> (sfb_end - sfb_start) */

    while(ics->_g < num_window_groups) {
        cb_p.group_len = group_lengths & 0x0f;

        while(ics->_k < b.len) {

            idx = maac_section_idx(ics->_g, ics->_i);
            cb_p.cb = ics->section_data[idx].codebook;
            cb_p.sfb_start = b.offsets[ics->_k];
            cb_p.sfb_end   = b.offsets[ics->_k + 1];
            switch(cb_p.cb) {
                case MAAC_ZERO_HCB: /* fall-through */
                case MAAC_INTENSITY_HCB: /* fall-through */
                case MAAC_INTENSITY_HCB2: {
                    if(p->ch != NULL) maac_ics_zero_codebook(ics, &cb_p);
                    break;
                }
                case MAAC_NOISE_HCB: {
                    /* this is handled in a higher layer (sce/cpe) */
                    break;
                }
                default: {
                    if( (res = maac_ics_default_codebook(ics, br, &cb_p)) != MAAC_OK) return res;
                    break;
                }
            }
            ics->_k++;
            if(ics->_k == ics->section_data[idx].end) {
                ics->_i++;
            }
        }

        ics->_group_off += ((maac_u16)cb_p.group_len) * 128;
        group_lengths >>= 4;
        ics->_g++;
        ics->_i = 0;
        ics->_k = 0;
    }

    return MAAC_OK;
}

static MAAC_RESULT
maac_scale_factor_parse(maac_ics* ics,  maac_bitreader* maac_restrict br, const maac_ics_decode_params* p) {
    const maac_ics_info* info = &ics->info;
    const maac_u8 num_window_groups = maac_sfg_num_window_groups(info->scale_factor_grouping);
    const maac_scalefactor_bands b = maac_scalefactor_bandsf(info->window_sequence, p->sf_index);

    MAAC_RESULT res;
    maac_u8 cb;
    maac_u32 idx;
    maac_s16 s16tmp;
    maac_u16 off;

    while(ics->_g < num_window_groups) {
        off = maac_section_idx(ics->_g, 0);

        while(ics->_k < b.len) {
            idx = maac_section_idx(ics->_g, ics->_i);
            cb = ics->section_data[idx].codebook;

            while(ics->_k < ics->section_data[idx].end) {
                switch(cb) {
                    case MAAC_ZERO_HCB: {
                        ics->scalefactors[off + ics->_k] = 0;
                        break;
                    }
                    case MAAC_INTENSITY_HCB: /* fall-through */
                    case MAAC_INTENSITY_HCB2: {
                        if( (res = maac_huffman_decode(&ics->_huffman, br, 0)) != MAAC_OK) return res;

                        ics->_dpcm_is_position += ((maac_s16)ics->_huffman.index) - 60;
                        ics->scalefactors[off + ics->_k] = ics->_dpcm_is_position;
                        break;
                    }
                    case MAAC_NOISE_HCB: {
                        if(ics->_noise_flag) {
                            if( (res = maac_bitreader_fill(br, 9)) != MAAC_OK) return res;
                            s16tmp = (maac_s16)maac_bitreader_read(br, 9);
                            s16tmp -= ((maac_s16)(1 << 8));
                            ics->_noise_flag = 0;
                        } else {
                            if( (res = maac_huffman_decode(&ics->_huffman, br, 0)) != MAAC_OK) return res;
                            s16tmp = ((maac_s16)ics->_huffman.index) - 60;
                        }
                        ics->_noise_energy += s16tmp;
                        ics->scalefactors[off + ics->_k] = ics->_noise_energy;
                        break;
                    }
                    default: {
                        if( (res = maac_huffman_decode(&ics->_huffman, br, 0)) != MAAC_OK) return res;
                        ics->_scale_factor += ((maac_u8)ics->_huffman.index) - 60;
                        ics->scalefactors[off + ics->_k] = ics->_scale_factor;
                        break;
                    }
                }
                ics->_k++;
            }

            ics->_i++;
        }

        ics->_g++;
        ics->_k = 0;
        ics->_i = 0;
    }
    return MAAC_OK;
}


static MAAC_RESULT
maac_section_data_parse(maac_ics* ics, maac_bitreader* maac_restrict br, const maac_ics_decode_params* p) {
    const maac_ics_info* info = &ics->info;
    const maac_u8 esc_val =
      info->window_sequence == MAAC_WINDOW_SEQUENCE_EIGHT_SHORT ? 0x07 : 0x1f;
    const maac_u8 sect_bits =
      esc_val == 0x07 ? 3 : 5;
    const maac_u8 num_window_groups = maac_sfg_num_window_groups(info->scale_factor_grouping);
    const maac_scalefactor_bands b = maac_scalefactor_bandsf(info->window_sequence, p->sf_index);

    MAAC_RESULT res;
    maac_u8 sect_len_incr;
    maac_u32 idx;

    while(ics->_g < num_window_groups) {
        switch(ics->state) {
            case MAAC_ICS_STATE_SECTION_CODEBOOK: {
                maac_ics_state_section_codebook:
                idx = maac_section_idx(ics->_g, ics->_i);

                if( (res = maac_bitreader_fill(br, 4)) != MAAC_OK) return res;
                ics->section_data[idx].codebook = maac_bitreader_read(br, 4);
                ics->section_data[idx].end = 0;
                ics->state = MAAC_ICS_STATE_SECTION_CODEBOOK_LENGTH;
                goto maac_ics_state_section_codebook_length;
            }

            case MAAC_ICS_STATE_SECTION_CODEBOOK_LENGTH: {
                maac_ics_state_section_codebook_length:
                idx = maac_section_idx(ics->_g, ics->_i);

                if( (res = maac_bitreader_fill(br, sect_bits)) != MAAC_OK) return res;
                sect_len_incr = maac_bitreader_read(br, sect_bits);
                ics->section_data[idx].end += sect_len_incr;
                if(sect_len_incr == esc_val) goto maac_ics_state_section_codebook_length;

                ics->state = MAAC_ICS_STATE_SECTION_CODEBOOK;

                /* previous our "end" value was a length, replace with _k to become an absolute position */
                ics->_k += ics->section_data[idx].end;

                ics->section_data[idx].end = ics->_k;
                ics->_i++;

                if(ics->_k < info->max_sfb) goto maac_ics_state_section_codebook;

                if(b.len > info->max_sfb) {
                    ics->section_data[maac_section_idx(ics->_g,ics->_i)].codebook = 0;
                    ics->section_data[maac_section_idx(ics->_g,ics->_i)].end =
                      b.len;
                }
                break;
            }
            default: {
                MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
            }
        }

        ics->_k = 0;
        ics->_i = 0;
        ics->_g++;
    }

    return MAAC_OK;
}

MAAC_PUBLIC
MAAC_RESULT
maac_ics_decode(maac_ics* ics, maac_bitreader* maac_restrict br, const maac_ics_decode_params* p) {
    const maac_ics_info* info = &ics->info;
    MAAC_RESULT res;

    switch(ics->state) {
        case MAAC_ICS_STATE_GLOBAL_GAIN: {
            if( (res = maac_bitreader_fill(br,8)) != MAAC_OK) return res;
            ics->global_gain = maac_bitreader_read(br,8);
            if(p->common_window) {
                ics->state = MAAC_ICS_STATE_SECTION_CODEBOOK;
                maac_ics_reset_iterators(ics);
                goto maac_ics_state_section_codebook;
            }
            ics->state = MAAC_ICS_STATE_ICS_INFO;
            maac_ics_info_init(&ics->info);
            goto maac_ics_state_ics_info;
        }

        case MAAC_ICS_STATE_ICS_INFO: {
            maac_ics_state_ics_info:
            if( (res =  maac_ics_info_parse(&ics->info, br, p->sf_index)) != MAAC_OK) return res;
            ics->state = MAAC_ICS_STATE_SECTION_CODEBOOK;
            maac_ics_reset_iterators(ics);
            goto maac_ics_state_section_codebook;
        }

        case MAAC_ICS_STATE_SECTION_CODEBOOK: /* fall-through */
        case MAAC_ICS_STATE_SECTION_CODEBOOK_LENGTH: {
            maac_ics_state_section_codebook:
            if( (res = maac_section_data_parse(ics, br, p)) != MAAC_OK) return res;

            ics->state = MAAC_ICS_STATE_SCALE_FACTOR_DATA;
            maac_ics_reset_iterators(ics);
            maac_ics_reset_scalefactors(ics);
            maac_huffman_init(&ics->_huffman);

            goto maac_ics_state_scale_factor_data;
        }

        case MAAC_ICS_STATE_SCALE_FACTOR_DATA: {
            maac_ics_state_scale_factor_data:
            if( (res = maac_scale_factor_parse(ics, br, p)) != MAAC_OK) return res;
            ics->state = MAAC_ICS_STATE_PULSE_DATA_PRESENT;
            goto maac_ics_state_pulse_data_present;
        }

        case MAAC_ICS_STATE_PULSE_DATA_PRESENT: {
            maac_ics_state_pulse_data_present:
            if( (res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            ics->pulse_data_present = (maac_u8)maac_bitreader_read(br, 1);
            if(ics->pulse_data_present) {
                ics->state = MAAC_ICS_STATE_PULSE_DATA;
                maac_pulse_init(&ics->pulse);
                goto maac_ics_state_pulse_data;
            }
            ics->state = MAAC_ICS_STATE_TNS_DATA_PRESENT;
            goto maac_ics_state_tns_data_present;
        }

        case MAAC_ICS_STATE_PULSE_DATA: {
            maac_ics_state_pulse_data:
            if( (res = maac_pulse_parse(&ics->pulse, br)) != MAAC_OK) return res;

            ics->state = MAAC_ICS_STATE_TNS_DATA_PRESENT;
            goto maac_ics_state_tns_data_present;
        }

        case MAAC_ICS_STATE_TNS_DATA_PRESENT: {
            maac_ics_state_tns_data_present:
            if( (res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            ics->tns_data_present = (maac_u8)maac_bitreader_read(br, 1);
            if(ics->tns_data_present) {
                ics->tns._g = 0;
                ics->tns.state = MAAC_TNS_STATE_N_FILT;
                ics->state = MAAC_ICS_STATE_TNS_DATA;
                goto maac_ics_state_tns_data;
            }
            ics->state = MAAC_ICS_STATE_GAIN_CONTROL_DATA_PRESENT;
            goto maac_ics_state_gain_control_data_present;
        }

        case MAAC_ICS_STATE_TNS_DATA: {
            maac_ics_state_tns_data:
            if( (res = maac_tns_parse(&ics->tns, br, info->window_sequence)) != MAAC_OK) return res;

            ics->state = MAAC_ICS_STATE_GAIN_CONTROL_DATA_PRESENT;
            goto maac_ics_state_gain_control_data_present;
        }

        case MAAC_ICS_STATE_GAIN_CONTROL_DATA_PRESENT: {
            maac_ics_state_gain_control_data_present:
            if( (res = maac_bitreader_fill(br,1)) != MAAC_OK) return res;
            ics->gain_control_data_present = (maac_u8)maac_bitreader_read(br, 1);
            maac_assert(ics->gain_control_data_present == 0);
            if(ics->gain_control_data_present) {
                ics->state = MAAC_ICS_STATE_GAIN_CONTROL_DATA;
                return MAAC_GAIN_CONTROL_DATA_NOT_IMPLEMENTED;
            }

            ics->state = MAAC_ICS_STATE_SPECTRAL_DATA;
            maac_ics_reset_iterators(ics);
            maac_huffman_init(&ics->_huffman);
            goto maac_ics_state_spectral_data;
        }

        case MAAC_ICS_STATE_GAIN_CONTROL_DATA: {
            ics->state = MAAC_ICS_STATE_SPECTRAL_DATA;
            return MAAC_GAIN_CONTROL_DATA_NOT_IMPLEMENTED;
        }

        case MAAC_ICS_STATE_SPECTRAL_DATA: {
            maac_ics_state_spectral_data:
            if( (res = maac_spectra_decode(ics, br, p)) != MAAC_OK) return res;
            ics->state = MAAC_ICS_STATE_GLOBAL_GAIN;
            return MAAC_OK;
        }
    }

    MAAC_UNREACHABLE_RETURN(MAAC_UNREACHABLE);
}
