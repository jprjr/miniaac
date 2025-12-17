#include <stdio.h>
#include <math.h>
#include <float.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float d(unsigned int n) {
    return (float)n;
}

static void generate_twiddles(char c, unsigned int n) {
    const unsigned int n_lim = c == 'C' ? (n / 8) : (n / 4);

    unsigned int k = 0;
    float t1, t2;

    printf("static const maac_flt MAAC_IMDCT_%c_%u[%u] = {", c, n, c == 'C' ? (n / 4) : (n / 2));

    for(k=0; k < n_lim; k++) {
        if(k > 0) {
            printf(",");
        }

        switch(c) {
            case 'A': {
                t1 =  cos(  (d(4 * k) * M_PI) / d(n) );
                t2 = -sin(  (d(4 * k) * M_PI) / d(n) );
                break;
            }
            case 'B': {
                t1 =  cos( (d(2 * k + 1) * M_PI) / d(2 * n) );
                t2 =  sin( (d(2 * k + 1) * M_PI) / d(2 * n) );
                break;
            }
            case 'C': {
                t1 =  cos( (d(2 * (2 * k + 1)) * M_PI) / d(n) );
                t2 = -sin( (d(2 * (2 * k + 1)) * M_PI) / d(n) );
                break;
            }
            default: break;
        }

        printf("\n    MAAC_FLT_C(%#.*g), MAAC_FLT_C(%#.*g)", DBL_DECIMAL_DIG, t1, DBL_DECIMAL_DIG, t2);
    }
    printf("\n};\n\n");
}

int main(void) {
    printf("/* SPDX-License-Identifier: 0BSD */\n");
    printf("#ifndef MAAC_IMDCT_TWIDDLE_FACTORS_INCLUDE_GUARD\n");
    printf("#define MAAC_IMDCT_TWIDDLE_FACTORS_INCLUDE_GUARD\n\n");
    printf("#include \"maac_float.h\"\n\n");
    generate_twiddles('A',2048);
    generate_twiddles('B',2048);
    generate_twiddles('C',2048);

    generate_twiddles('A',256);
    generate_twiddles('B',256);
    generate_twiddles('C',256);
    printf("\n#endif /* INCLUDE_GUARD */\n");
    return 0;
}
