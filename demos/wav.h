/* SPDX-License-Identifier: 0BSD */
#include <stdio.h>
#include "maac.h"

int wav_header_create(FILE* output, maac_u32 sample_rate, maac_u32 channels, maac_u32 bit_depth);
int wav_header_finish(FILE* output, maac_u32 bit_depth);
