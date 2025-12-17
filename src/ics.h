/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_INDIVIDUAL_CHANNEL_STREAM_INCLUDE_GUARD
#define MAAC_INDIVIDUAL_CHANNEL_STREAM_INCLUDE_GUARD

#include "maac_common.h"
#include "maac_consts.h"
#include "bitreader.h"
#include "bitfield.h"
#include "channel.h"

#include "ics_info.h"
#include "tns.h"
#include "huffman.h"
#include "pulse.h"

enum MAAC_ICS_STATE {
    MAAC_ICS_STATE_GLOBAL_GAIN               =  0,
    MAAC_ICS_STATE_ICS_INFO                  =  1,
    MAAC_ICS_STATE_SECTION_CODEBOOK          =  2,
    MAAC_ICS_STATE_SECTION_CODEBOOK_LENGTH   =  3,
    MAAC_ICS_STATE_SCALE_FACTOR_DATA         =  4,
    MAAC_ICS_STATE_PULSE_DATA_PRESENT        =  5,
    MAAC_ICS_STATE_PULSE_DATA                =  6,
    MAAC_ICS_STATE_TNS_DATA_PRESENT          =  7,
    MAAC_ICS_STATE_TNS_DATA                  =  8,
    MAAC_ICS_STATE_GAIN_CONTROL_DATA_PRESENT =  9,
    MAAC_ICS_STATE_GAIN_CONTROL_DATA         = 10,
    MAAC_ICS_STATE_SPECTRAL_DATA             = 11
};

typedef enum MAAC_ICS_STATE MAAC_ICS_STATE;

struct maac_section_data {
    maac_u8 codebook; /* 4 bits */
    maac_u8      end; /* 6 bits */
};
typedef struct maac_section_data maac_section_data;

struct maac_ics {
    MAAC_ICS_STATE state;

    maac_ics_info info;
    maac_huffman _huffman;

    maac_section_data section_data[MAAC_MAX_SECTIONS];

    maac_s16 scalefactors[MAAC_MAX_SECTIONS];

    maac_pulse pulse;
    maac_tns tns;

    maac_s16 spectra_tmp[4];

    maac_u8 global_gain;
    maac_u8 pulse_data_present;
    maac_u8 tns_data_present;
    maac_u8 gain_control_data_present;

    maac_u8  _g; /* used to iterate num_window_groups */
    maac_u8  _w;
    maac_u16 _i;
    maac_u16 _k;
    maac_u16 _p;
    maac_u16 _off;
    maac_u16 _group_off;
    maac_u8 _noise_flag;
    maac_s16 _dpcm_is_position;
    maac_u8 _scale_factor;
    maac_s32 _noise_energy;
};

typedef struct maac_ics maac_ics;

struct maac_ics_decode_params {
    maac_u32 sf_index;
    maac_u8 common_window;
    maac_channel *ch;
};

typedef struct maac_ics_decode_params maac_ics_decode_params;

MAAC_CDECLS_BEGIN

MAAC_PUBLIC
void
maac_ics_init(maac_ics* ics);

MAAC_PUBLIC
MAAC_RESULT
maac_ics_decode(maac_ics* ics, maac_bitreader* maac_restrict br, const maac_ics_decode_params* p);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
