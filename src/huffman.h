/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_HUFFMAN_INCLUDE_GUARD
#define MAAC_HUFFMAN_INCLUDE_GUARD

#include "maac_common.h"
#include "bitreader.h"

/* the state is really just used for decoding spectral data,
for scalefactor we just always decode from book "0" and process
the index directly (no sign bits, escape codes) */
enum MAAC_HUFFMAN_STATE {
    MAAC_HUFFMAN_STATE_CODEWORD,
    MAAC_HUFFMAN_STATE_SIGN_BITS,
    MAAC_HUFFMAN_STATE_ESC_PREFIX,
    MAAC_HUFFMAN_STATE_ESC
};

typedef enum MAAC_HUFFMAN_STATE MAAC_HUFFMAN_STATE;

struct maac_huffman {
    MAAC_HUFFMAN_STATE state;
    maac_u8 bits; /* the index of how many bits we're trying to read */
    maac_u8 esc;
    maac_u16 offset; /* our current offset into the codebook */
    maac_u32 codeword; /* the decoded codeword */
    maac_u32 index; /* the codeword's index */
};

typedef struct maac_huffman maac_huffman;

MAAC_PRIVATE
void
maac_huffman_init(maac_huffman* h);

MAAC_PRIVATE
MAAC_RESULT
maac_huffman_decode(maac_huffman* h, maac_bitreader* br, maac_u8 codebook);

MAAC_PRIVATE
MAAC_RESULT
maac_huffman_decode_spectral(maac_huffman* h, maac_bitreader* br, maac_u8 codebook, maac_s16 out[4]);

#endif /* INCLUDE_GUARD */
