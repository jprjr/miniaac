/* SPDX-License-Identifier: 0BSD */
/* #define MAAC_COMPACT */
#include "maac.h"
#include "maac_extras.h"

#define MAAC_IMPLEMENTATION
#include "maac.h"

#define MAAC_EXTRAS_IMPLEMENTATION
#include "maac_extras.h"

#ifdef __cplusplus
#include <cstdio>
#include <cassert>
#else
#include <stdio.h>
#include <assert.h>
#endif

#if (defined(__cplusplus) && (__cplusplus >= 201103L)) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)

#if defined(__cplusplus) && __cplusplus >= 201103L
#define maac_alignof(x) alignof(x)
#elif __STDC_VERSION__ >= 202311L
#define maac_alignof(x) alignof(x)
#else
#include <stdalign.h>
#define maac_alignof(x) _Alignof(x)
#endif

#define t_align(x) \
; fprintf(stdout,"alignof(maac_" #x "): %u\n", (unsigned int)maac_alignof(maac_ ## x)); \
assert(maac_alignof(maac_ ## x) == maac_ ## x ## _alignof())

#else
#define t_align(x)
#endif

#define t(x) \
fprintf(stdout, "sizeof(maac_" #x "): %u\n", (unsigned int)sizeof(maac_ ## x)); \
fprintf(stdout, "maac_" #x "_size(): %u\n", (unsigned int)maac_ ## x ## _size()); \
assert(sizeof(maac_ ## x) == maac_ ## x ## _size()); \
fprintf(stdout, "maac_" #x "_alignof(): %u\n", (unsigned int)maac_ ## x ## _alignof()) \
t_align(x)

#define d(x) \
const_sizes += sizeof(maac_ ## x); \
fprintf(stdout, "sizeof(maac_" #x "): %u\n", (unsigned int)sizeof(maac_ ## x))

#define D(x) \
const_sizes += sizeof(MAAC_ ## x); \
fprintf(stdout, "sizeof(MAAC_" #x "): %u\n", (unsigned int)sizeof(MAAC_ ## x))

#define tf(t, f) fprintf(stdout,"sizeof(maac_" #t "." #f "): %u\n", (unsigned int) sizeof( (maac_ ## t*) NULL)->f);

int main(void) {
    size_t const_sizes = 0;

    fprintf(stdout,"struct sizes\n");
    t(bitreader);
    t(adts);
    t(raw);
    t(sce);
    t(cpe);
    t(fil);
    t(ics);
    t(ics_info);

    fprintf(stdout,"\nmaac_ics field sizes:\n");
    tf(ics, state);
    tf(ics, info);
    tf(ics, _huffman);
    tf(ics, section_data);
    tf(ics, scalefactors);
    tf(ics, pulse);
    tf(ics, tns);
    tf(ics, spectra_tmp);
    tf(ics, global_gain);
    tf(ics, pulse_data_present);
    tf(ics, tns_data_present);
    tf(ics, gain_control_data_present);
    tf(ics, _g);
    tf(ics, _w);
    tf(ics, _i);
    tf(ics, _k);
    tf(ics, _p);
    tf(ics, _off);
    tf(ics, _group_off);
    tf(ics, _noise_flag);
    tf(ics, _dpcm_is_position);
    tf(ics, _scale_factor);
    tf(ics, _noise_energy);

    fprintf(stdout,"\nconsts sizes:\n");

    d(codebook_bits_indexes);
    d(codebook_bits);
    d(codebook_indexes);
    d(codebook);

    d(num_swb_long_window);
    d(num_swb_short_window);
    d(swb_offset_long_window);
    d(swb_offset_long_window_index);
    d(swb_offset_short_window);
    d(swb_offset_short_window_index);

    d(bit_reverse_data);

    D(IMDCT_A_2048);
    D(IMDCT_B_2048);
    D(IMDCT_C_2048);

    D(IMDCT_A_256);
    D(IMDCT_B_256);
    D(IMDCT_C_256);

    d(window_kbd_1024);
    d(window_sin_1024);

    d(window_kbd_128);
    d(window_sin_128);

#ifdef MAAC_INVQANT_TABLES
    D(INV_QUANT);
#endif

    fprintf(stderr,"\ntotal const data: %lu\n", (long unsigned)const_sizes);

    return 0;
}

