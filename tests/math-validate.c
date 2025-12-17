#include "maac_math.h"

#ifdef __cplusplus
#include <cstdio>
#include <cmath>
#include <cfloat>
#include <cstring>
#else
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
#endif

static int verbose = 0;

static double pow2_div4_ref(maac_s16 x) {
    return pow(2.0, ((double)x) * 0.25);
}

static double cbrt_ref(maac_u16 x) {
    return pow((double)x,1.0/3.0);
}

static double pow2_t(maac_s16 x) {
    if(verbose) fprintf(stderr,"pow2: reference value: %.*g\n", 17, pow2_div4_ref(x));
    if(verbose) fprintf(stderr,"pow2 maac value: %.*g\n", 17, maac_pow2_xdiv4(x));
    return fabs(pow2_div4_ref(x) - maac_pow2_xdiv4(x));
}

static double cbrt_t(maac_u16 x) {
    if(verbose) fprintf(stderr,"cbrt(%d) reference value: %.*g\n", x, 17, cbrt_ref(x));
    if(verbose) fprintf(stderr,"cbrt(%d) maac value: %.*g\n", x, 17, maac_cbrt(x));
    return fabs(cbrt_ref(x) - maac_cbrt(x));
}

int main(int argc, const char* argv[]) {
    int i;
    int max_i;
    double error;
    double avg_error;
    double max_error;

    if(argc > 1 && strcmp(argv[1],"-v") == 0) {
        verbose = 1;
    }

    max_i = 0;
    error = 0.0;
    avg_error = 0.0;
    max_error = 0.0;
    for(i = -155; i < 155; i++) {
        error = pow2_t(i);
        avg_error += error;
        if(error > max_error) {
            max_error = error;
            max_i = i;
        }
    }
    avg_error /= 310.0;
    fprintf(stderr,"pow2: average error: %.*g\n", 17, avg_error);
    fprintf(stderr,"pow2: max error: %.*g (i=%d, ref=%.*g maac=%.*g)\n", 17, max_error, max_i, pow2_div4_ref(max_i), maac_pow2_xdiv4(max_i));

    max_i = 0;
    error = 0.0;
    avg_error = 0.0;
    max_error = 0.0;
    for(i=0;i<8192;i++) {
        error = cbrt_t(i);
        avg_error += error;
        if(error > max_error) {
            max_error = error;
            max_i = i;
        }
    }
    avg_error /= 8192.0;
    fprintf(stderr,"cbrt: average error: %.*g\n", 17, avg_error);
    fprintf(stderr,"cbrt: max error: %.*g (i=%d, ref=%.*g maac=%.*g)\n", 17, max_error, max_i, cbrt_ref(max_i), maac_cbrt(max_i));

    return 0;
}

    
