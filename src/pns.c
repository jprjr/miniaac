#include "pns.h"

#include "maac_math.h"
#include "maac_rand.h"
#include "scalefactor_bands.h"
#include "window_group_lengths.h"
#include "sfg_num_window_groups.h"
#include "maac_section_idx.h"


MAAC_PRIVATE
void
maac_pns_process(const maac_pns_params* p) {
    const maac_ics* ics = p->ics;
    const maac_ics_info* info = &ics->info;
    const maac_u8 num_window_groups = maac_sfg_num_window_groups(info->scale_factor_grouping);
    const maac_scalefactor_bands b = maac_scalefactor_bandsf(info->window_sequence, p->sf_index);
    maac_u32 group_lengths = maac_window_group_lengths(info->window_sequence, info->scale_factor_grouping);

    maac_u8 g;
    maac_u8 group_len;
    maac_u8 w;
    maac_u8 k;
    maac_u8 i;
    maac_u8 idx;
    maac_u16 off;
    maac_u16 group_off;
    maac_u16 sfb;
    maac_u16 sfb_start;
    maac_u16 sfb_end;
    maac_s32 r;
    maac_flt sample;
    maac_flt energy;
    maac_flt scale;

    group_off = 0;
    for(g=0; g<num_window_groups; g++) {
        group_len = group_lengths & 0x0f;

        for(w=0; w<group_len; w++) {
            i = 0;
            k = 0;

            off = group_off + ((maac_u16)w) * 128;

            while(k < b.len) {
                idx = maac_section_idx(g, i);

                if(ics->section_data[idx].codebook == MAAC_NOISE_HCB) {
                    scale = maac_pow2_xdiv4(ics->scalefactors[maac_section_idx(g,k)]);
                    energy = MAAC_FLT_C(0.0);
                    sfb_start = b.offsets[k];
                    sfb_end = b.offsets[k+1];

                    for(sfb=sfb_start;sfb<sfb_end;sfb++) {
                        r = (maac_s32)maac_rand(p->rand_state);
                        sample = maac_flt_cast(r);
                        p->spectra[off + sfb] = sample;
                        energy += sample * sample;
                    }
                    energy = scale * maac_inv_sqrt(energy);
                    for(sfb=sfb_start;sfb<sfb_end;sfb++) {
                        p->spectra[off + sfb] *= energy;
                    }
                    k++;
                } else {
                    /* skip to end of section */
                    k = ics->section_data[idx].end;
                }

                if(k == ics->section_data[idx].end) {
                    i++;
                }
            }
        }

        group_off += ((maac_u16)group_len) * 128;
        group_lengths >>=4;
    }
}
