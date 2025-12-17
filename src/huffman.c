/* SPDX-License-Identifier: 0BSD */
#include "huffman.h"
#include "huffman_codebooks.h"
#include "bitreader_private.h"

#include "maac_memset.h"
#include "maac_abs.h"
#include "maac_consts.h"

static const maac_u8 maac_codebook_params[12] = {
  0x00,
  0x61,
  0x61,
  0xe2,
  0xe2,
  0x24,
  0x24,
  0xa7,
  0xa7,
  0xac,
  0xac,
  0xb0
};

#define maac_codebook_unsigned(x) (maac_codebook_params[(x)] >> 7)
#define maac_codebook_dimension(x) (1 + ((maac_codebook_params[(x)] >> 5) & 0x03))
#define maac_codebook_lav(x) (maac_codebook_params[(x)] & 0x1f)

MAAC_PRIVATE
void
maac_huffman_init(maac_huffman* h) {
    maac_memset(h, 0, sizeof *h);
}

MAAC_PRIVATE
MAAC_RESULT
maac_huffman_decode(maac_huffman* h, maac_bitreader* br, maac_u8 cb) {
    MAAC_RESULT res;
    maac_u16 i;
    maac_u16 cw_start;
    maac_u16 cw_end;

    maac_u32 cw;

    /* TODO with compact codebooks we do multiplying by stride to get to indexes, I think
    this could be tweaked to where h->offset is the actual number of bytes and we do
    everything by h->offset += maac_codebook_stride(cb) */

    while(maac_codebook_bits_indexes[cb].start + h->bits < maac_codebook_bits_indexes[cb].end) {

        if(maac_codebook_bits[maac_codebook_bits_indexes[cb].start + h->bits]) {

            if( (res = maac_bitreader_fill(br, h->bits + 1)) != MAAC_OK) return res;

            cw = maac_bitreader_peek(br, h->bits + 1);
            cw_start = h->offset;
            cw_end = h->offset + ((maac_u16)maac_codebook_bits[maac_codebook_bits_indexes[cb].start + h->bits]);

            for(i=cw_start; i<cw_end; i++) {
                if(cw == maac_codebook_codeword(cb, i)) {
                    /* reset some fields for the next go-around */
                    maac_bitreader_discard(br, h->bits + 1);
                    h->bits = 0;
                    h->offset = 0;
                    h->codeword = cw;
                    h->index = maac_codebook_index(cb, i);
                    return MAAC_OK;
                }
            }
            h->offset = cw_end;
        }
        h->bits++;
    }

    return MAAC_HUFFMAN_DECODE_ERROR;
}

#define maac_huffman_esc(val) ( maac_abs(val) == MAAC_ESC_FLAG ? 1 : 0 )

static void maac_spectral_data_calc(maac_u8 cb, maac_u32 index, maac_s16* out) {
    maac_s32 sidx = 0;
    maac_s32 mod = 0;
    maac_s32 off = 0;
    maac_s32 lav = 0;
    maac_s32 _w = 0;
    maac_s32 _x = 0;
    maac_s32 _y = 0;
    maac_s32 _z = 0;

    sidx = (maac_s32)index;
    lav = maac_codebook_lav(cb);

    if(maac_codebook_unsigned(cb)) {
        mod = lav + 1;
    } else {
        mod = 2 * lav + 1;
        off = lav;
    }

    if(maac_codebook_dimension(cb) == 4) {
        _w = (sidx / (mod * mod * mod) ) - off;
        sidx -= (_w + off) * (mod * mod * mod);

        _x = (sidx / (mod * mod) ) - off;
        sidx -= (_x + off) * (mod * mod);
    }

    _y = (sidx / mod) - off;
    sidx -= (_y + off) * mod;

    _z = sidx - off;

    if(maac_codebook_dimension(cb) == 4) {
        out[0] = _w;
        out[1] = _x;
        out[2] = _y;
        out[3] = _z;
    } else {
        out[0] = _y;
        out[1] = _z;
    }
}

MAAC_PRIVATE
MAAC_RESULT
maac_huffman_decode_spectral(maac_huffman* h, maac_bitreader* br, maac_u8 cb, maac_s16 out[4]) {
    MAAC_RESULT res;
    maac_u8 signbits;
    maac_u8 i;
    maac_s16 esc;

    switch(h->state) {
        case MAAC_HUFFMAN_STATE_CODEWORD: {
            if( (res = maac_huffman_decode(h, br, cb)) != MAAC_OK) return res;
            maac_spectral_data_calc(cb, h->index, out);

            if(maac_codebook_unsigned(cb)) {
                h->state = MAAC_HUFFMAN_STATE_SIGN_BITS;
                goto maac_huffman_state_sign_bits;
            }

            if(cb == MAAC_ESC_HCB) goto maac_check_esc_y;
            break;
        }

        case MAAC_HUFFMAN_STATE_SIGN_BITS: {
            maac_huffman_state_sign_bits:
            signbits = 0;
            for(i = 0; i < maac_codebook_dimension(cb); i++) {
                if(out[i]) signbits++;
            }
            if(signbits) {
                if( (res = maac_bitreader_fill(br, signbits)) != MAAC_OK) return res;
                for(i = 0; i < maac_codebook_dimension(cb); i++) {
                    if(out[i]) {
                        if(maac_bitreader_read(br,1)) {
                            out[i] = -out[i];
                        }
                        if(--signbits == 0) break;
                    }
                }
            }

            if(cb == MAAC_ESC_HCB) goto maac_check_esc_y;
            break;
        }

        case MAAC_HUFFMAN_STATE_ESC_PREFIX: {
            maac_huffman_state_esc_prefix:
            if( (res = maac_bitreader_fill(br, 1)) != MAAC_OK) return res;
            if(maac_bitreader_read(br,1)) {
                h->bits++;
                goto maac_huffman_state_esc_prefix;
            }
            h->state = MAAC_HUFFMAN_STATE_ESC;
        }
        /* fall-through */
        case MAAC_HUFFMAN_STATE_ESC: {
            if( (res = maac_bitreader_fill(br, h->bits + 4)) != MAAC_OK) return res;
            esc = (maac_s16) ((1 << (h->bits + 4)) + maac_bitreader_read(br, h->bits + 4));
            h->bits = 0;
            out[h->esc] = out[h->esc] < 0 ? -esc : esc;
            if(h->esc == 0) goto maac_check_esc_z;
            break;
        }
    }

    goto maac_huffman_spectral_complete;

    maac_check_esc_y: 
    if(maac_huffman_esc(out[0])) {
        h->state = MAAC_HUFFMAN_STATE_ESC_PREFIX;
        h->esc = 0;
        goto maac_huffman_state_esc_prefix;
    }

    maac_check_esc_z: 
    if(maac_huffman_esc(out[1])) {
        h->state = MAAC_HUFFMAN_STATE_ESC_PREFIX;
        h->esc = 1;
        goto maac_huffman_state_esc_prefix;
    }

    maac_huffman_spectral_complete:
    h->state = MAAC_HUFFMAN_STATE_CODEWORD;
    h->bits = 0;
    h->esc = 0;
    return MAAC_OK;
}
