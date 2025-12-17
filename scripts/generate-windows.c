#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#define STR(x) #x
#define XSTR(x) STR(x)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double i0(double x) {
    double fact = 1.0;
    double sum = 0;
    unsigned int k = 0;

    do {
        sum += pow(x/2, 2*k) / pow(fact, 2);
        fact *= ++k;
    }  while(k < 64);

    return sum;
}

static double kaiser_window(double alpha, double n, double N) {
    return i0(
      M_PI * alpha * sqrt(1.0 - pow( (n - N/4.0) / (N/4.0) , 2.0f))
    ) / i0( M_PI * alpha);
}

static double kbd_window(unsigned int alpha, unsigned int n, unsigned int N, double denom) {
    unsigned int i = 0;
    double sum1 = 0.0;

    if(n < N/2) {
        for(i=0;i<=n; i++) {
            sum1 += kaiser_window((double)alpha, (double) i, (double) N);
        }
    } else {
        for(i=0;i<=N-n-1;i++) {
            sum1 += kaiser_window((double)alpha, (double) i, (double) N);
        }
    }
    return sqrt(sum1/denom);
}

static void generate_kbd(const char* name, unsigned int alpha, unsigned int N) {
    unsigned int i = 0;
    double denom = 0.0;

    for(i=0;i<=N/2;i++) {
        denom += kaiser_window((double)alpha, (double) i, (double) N);
    }

    fprintf(stdout, "static const maac_flt %s[%u] = {", name, N/2);
    for(i=0;i<N/2;i++) {
        if(i > 0) {
            fprintf(stdout, ",");
        }
        fprintf(stdout,"\n  MAAC_FLT_C(%.*g)", DBL_DECIMAL_DIG, kbd_window(alpha, i, N, denom));
    }
    fprintf(stdout,"\n};\n\n");

}

static double sin_window(double i, double N) {
    return sin(
      (M_PI / N) * (i + 0.5));
}

static void generate_sin(const char* name, unsigned int N) {
    unsigned int i = 0;

    fprintf(stdout, "static const maac_flt %s[%u] = {", name, N/2);
    for(i=0;i<N/2;i++) {
        if(i > 0) {
            fprintf(stdout,",");
        }
        fprintf(stdout,"\n  MAAC_FLT_C(%.*g)", DBL_DECIMAL_DIG, sin_window(i,N));
    }
    fprintf(stdout,"\n};\n\n");
}



int main(void) {
    fprintf(stdout, "/* SPDX-License-Identifier: 0BSD */\n");
    fprintf(stdout, "#ifndef MAAC_IMDCT_WINDOW_DATA_INCLUDE_GUARD\n");
    fprintf(stdout, "#define MAAC_IMDCT_WINDOW_DATA_INCLUDE_GUARD\n\n");
    fprintf(stdout, "#include \"maac_float.h\"\n\n");
    generate_kbd("maac_window_kbd_1024", 4, 2048);
    generate_kbd("maac_window_kbd_128", 6, 256);

    generate_sin("maac_window_sin_1024", 2048);
    generate_sin("maac_window_sin_128", 256);
    fprintf(stdout, "#endif /* INCLUDE_GUARD */\n");
    return 0;
}


