/* SPDX-License-Identifier: 0BSD */
#if !(defined(_WIN32) || defined(_WIN64))
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#endif

#include "imdct.h"

#include "maac_rand.h"
#include "maac_math.h"
#include "maac_memcpy.h"

#ifdef __cplusplus
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define USE_QPC
#include <windows.h>
#else
#define CLOCK_ID CLOCK_REALTIME
#endif

#ifdef USE_QPC
static LARGE_INTEGER frequency;
#else
static struct timespec timespec_diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}
#endif

/* calculate a value for a given index, using the formula
from the spec as-is - very slow */
static maac_flt reference_imdct_val(const maac_flt* in, maac_u16 len, maac_u16 idx) {
    unsigned int k = 0;
    maac_flt sum = MAAC_FLT_C(0.0);

    for(k=0;k < len/2; k++) {
        sum +=
          in[k] * cos(
            (MAAC_FLT_C(2.0) * MAAC_M_PI / maac_flt_cast(len))
            *
            (idx + ((maac_flt_cast(len) / MAAC_FLT_C(2.0)) + MAAC_FLT_C(1.0)) / MAAC_FLT_C(2.0))
            *
            (maac_flt_cast(k) + MAAC_FLT_C(0.5))
          );
    }

    sum *= MAAC_FLT_C(2.0) / maac_flt_cast(len);
    return sum;
}

static void reference_imdct(maac_flt* out, const maac_flt* in, maac_u16 len) {
    unsigned int i = len;
    while(i--) {
        out[i] = reference_imdct_val(in, len, i);
    }
}

static maac_flt in_values[1024];
static maac_flt out_values_reference[2048];
static maac_flt out_values_maac[2048];
static maac_u32 rand_state = 0xbabab00e;

static int test_imdct(maac_u16 len) {
    unsigned int i = 0;
    maac_s32 r;

    for(i = 0; i < len; i++) {
        if(i < (len/2)) {
            r = (maac_s32)maac_rand(&rand_state) % 8192;
            in_values[i] = maac_flt_cast(r) * maac_cbrt( (maac_u16)(r < 0 ? -r : r));
        }
        out_values_reference[i] = MAAC_FLT_C(0.0);
        out_values_maac[i]   = MAAC_FLT_C(0.0);
    }

    reference_imdct(out_values_reference, in_values, len);
    /* maac_imdct does everything in-place */
    memcpy(out_values_maac, in_values, sizeof(maac_flt) * (len / 2));
    maac_imdct(out_values_maac, len);

    fprintf(stderr,"# results for %u samples\n", len);
    fprintf(stderr,"| index |    reference |          maac |\n");
    for(i = 0; i < len; i++) {
        fprintf(stderr,"|  %04u | % 8.2f | % 8.2f |\n",
          i, out_values_reference[i], out_values_maac[i]);
    }

    return 0;
}

static void maac_imdct_bench(maac_flt* out, const maac_flt* in, maac_u16 len) {
    maac_imdct(out,len);
    (void)in;
}

static maac_u32 bench_imdct(void (*func)(maac_flt*,const maac_flt*, maac_u16), maac_u16 len, maac_u32 iterations) {
    unsigned int r = 0;
    unsigned int i = 0;

#ifdef USE_QPC
    LARGE_INTEGER time_start;
    LARGE_INTEGER time_end;
    LARGE_INTEGER time_total;
#else
    struct timespec time_start;
    struct timespec time_end;
    struct timespec time_total;
    struct timespec time_diff;
#endif
    maac_u32 time_avg;

#ifdef USE_QPC
    time_total.QuadPart = 0;
#else
    time_total.tv_sec = 0;
    time_total.tv_nsec = 0;
#endif

    for(r=0;r<iterations;r++) {
        for(i = 0; i < len; i++) {
            if(i < (len/2)) {
                in_values[i] = maac_flt_cast((maac_s32)maac_rand(&rand_state));
            }
            out_values_maac[i]   = MAAC_FLT_C(0.0);
            out_values_reference[i] = MAAC_FLT_C(0.0);
        }
        maac_memcpy(out_values_maac, in_values, sizeof(maac_flt) * len/2);

#ifdef USE_QPC
        QueryPerformanceCounter(&time_start);
#else
        clock_gettime(CLOCK_ID, &time_start);
#endif

        func(out_values_maac, in_values, len);

#ifdef USE_QPC
        QueryPerformanceCounter(&time_end);
        time_total.QuadPart += time_end.QuadPart - time_start.QuadPart;
#else
        clock_gettime(CLOCK_ID, &time_end);
        time_diff = timespec_diff(time_start,time_end);
        time_total.tv_sec += time_diff.tv_sec;
        time_total.tv_nsec += time_diff.tv_nsec;
        if(time_total.tv_nsec > 1000000000) {
            time_total.tv_nsec -= 1000000000;
            time_total.tv_sec++;
        }
#endif
    }

#ifdef USE_QPC
    time_total.QuadPart *= 1000000;
    time_total.QuadPart /= iterations;
    time_total.QuadPart /= frequency.QuadPart;
    time_avg = (maac_u32)time_total.QuadPart;
#else
    time_avg = (maac_u32)(time_total.tv_sec * 1000000); /* scale s up to us */
    time_avg += (maac_u32)time_total.tv_nsec / 1000; /* scale ns down to us */
    time_avg /= iterations;
#endif

    return time_avg;
}

const maac_u16 default_lengths[2] = { 2048 , 256 };

static int usage(const char* progname, int ret) {
    fprintf(stderr,"Usage: %s [-d] len (..len)\n", progname);
    return ret;
}

int main(int argc, const char* argv[]) {
    const char* progname = *argv;
    int rr = 0;
    int t;
    unsigned int i = 0;
    maac_u32 iterations = 100;
    maac_u32 ref_time;
    maac_u32 maac_time;

    const maac_u16* lengths = default_lengths;
    maac_u16* lengths_dyn = NULL;
    maac_u16 lengths_tot = sizeof(default_lengths) / sizeof(default_lengths[0]);

    argv++;
    argc--;

    while(argc) {
        if(strcmp(*argv,"-h") == 0) {
            return usage(progname, 0);
        } else if(strcmp(*argv,"--") == 0) {
            argc--;
            argv++;
            break;
        } else {
            break;
        }
    }

    if(argc) {
        lengths_dyn = (maac_u16*)malloc(sizeof(maac_u16) * argc);
        if(lengths_dyn == NULL) {
            fprintf(stderr,"failed to allocate lengths buffer\n");
        }
        lengths = lengths_dyn;
        lengths_tot = 0;

        while(argc) {
            t = atoi(*argv);
            if(t < 0) return usage(progname, 1);
            if(t > 2048) return usage(progname, 1);
            lengths_dyn[lengths_tot++] = (maac_u16) t;
            argc--;
            argv++;
        }
    }

    for(i=0; i < lengths_tot; i++) {
        fprintf(stderr,"testing length: %u\n", lengths[i]);
        test_imdct(lengths[i]);

        ref_time = bench_imdct(reference_imdct, lengths[i], iterations);
        fprintf(stderr,"reference average (%u iterations): %u us\n", iterations, ref_time);
        maac_time = bench_imdct(maac_imdct_bench, lengths[i], iterations);
        fprintf(stderr,"imdct average (%u iterations): %u us\n", iterations, maac_time);
    }


    if(lengths_dyn != NULL) {
        free(lengths_dyn);
    }

    return rr;
}

