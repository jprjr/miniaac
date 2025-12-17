#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* pre-compute coefficients to save a bunch of steps during TNS */

static const uint8_t sgn_mask[] = { 0x2, 0x4, 0x8 };
static const uint8_t neg_mask[] = { ~0x3, ~0x7, ~0xf };
static const uint8_t coef_bits[] = { 3, 4 };
static const uint8_t coef_compress[] = { 0, 1 };

int main(void) {
    double coef = 0.0;
    double iqfac = 0.0;
    double iqfac_m = 0.0;

    uint8_t i;
    uint8_t j;
    uint8_t bits;
    uint8_t b;
    uint8_t s_mask;
    uint8_t m_mask;
    int8_t  s;

    printf("/* SPDX-License-Identifier: 0BSD */\n");
    printf("#ifndef MAAC_TNS_INVQUANT_DATA_INCLUDE_GUARD\n");
    printf("#define MAAC_TNS_INVQUANT_DATA_INCLUDE_GUARD\n\n");
    printf("#include \"maac_float.h\"\n\n");


    for(i=0; i < (uint8_t)(sizeof(coef_compress) / sizeof(coef_compress[0])); i++) {
        for(j=0; j < (uint8_t)(sizeof(coef_bits) / sizeof(coef_bits[0])); j++) {
            printf("/* coef_compress = %u, coef_len = %u */\n", coef_compress[i], coef_bits[j]);
            printf("static const maac_flt MAAC_TNS_INVQUANT_%u_%u[] = {", coef_compress[i], coef_bits[j]);
            bits = coef_bits[j] - coef_compress[i];
            s_mask = sgn_mask[bits - 2];
            m_mask = neg_mask[bits - 2];
            iqfac = (((double)(( 1 << (coef_bits[j]-1)))) - 0.5 ) / (M_PI / 2.0);
            iqfac_m = (((double)(( 1 << (coef_bits[j]-1)))) + 0.5 ) / (M_PI / 2.0);

            for(b=0; b < (1 << bits); b++) {
                if(b > 0) {
                    printf(",");
                }

                s = b & s_mask ? (m_mask | b) : b;
                coef = sin(s / (s >= 0 ? iqfac : iqfac_m));

                printf("\n    /* value = %u (%d) */", b, s);
                printf(" MAAC_FLT_C(%#.*g)", DBL_DECIMAL_DIG, coef);
            }
            printf("\n};\n\n");
        }
    }

    printf("static const maac_flt* const MAAC_TNS_INVQUANT[] = {\n   ");
    for(i=0; i < (uint8_t)(sizeof(coef_compress) / sizeof(coef_compress[0])); i++) {
        for(j=0; j < (uint8_t)(sizeof(coef_bits) / sizeof(coef_bits[0])); j++) {
            if(i > 0 || j > 0) {
                printf(",");
            }
            printf(" MAAC_TNS_INVQUANT_%u_%u", coef_compress[i], coef_bits[j]);
        }
    }
    printf("\n};\n\n");

    printf("#endif /* INCLUDE_GUARD */\n");


    return 0;
}

