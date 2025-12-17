/* SPDX-License-Identifier: 0BSD */
#include "ics_extras.h"

#include "maac_align.h"

maac_const
MAAC_PUBLIC
size_t
maac_ics_size(void) {
    return sizeof(maac_ics);
}

struct maac_ics_aligner {
    char c;
    maac_ics ics;
};

maac_const
MAAC_PUBLIC
size_t
maac_ics_alignof(void) {
    return offsetof(struct maac_ics_aligner, ics);
}


maac_pure
MAAC_PUBLIC
maac_ics*
maac_ics_align(void* p) {
    return (maac_ics*)maac_align(p, offsetof(struct maac_ics_aligner, ics));
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_state(const maac_ics* ics) {
    return (maac_u32)ics->state;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_global_gain(const maac_ics* ics) {
    return (maac_u32)ics->global_gain;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_pulse_data_present(const maac_ics* ics) {
    return (maac_u32)ics->pulse_data_present;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_tns_data_present(const maac_ics* ics) {
    return (maac_u32)ics->tns_data_present;
}

maac_pure
MAAC_PUBLIC
maac_u32
maac_ics_gain_control_data_present(const maac_ics* ics) {
    return (maac_u32)ics->gain_control_data_present;
}

static const char* MAAC_ICS_STATE_INVALID_STR = "INVALID";
static size_t MAAC_ICS_STATE_INVALID_LEN = sizeof("INVALID") - 1;

static const char* const maac_ics_state_str_tbl[18] = {
    "GLOBAL_GAIN",
    "ICS_INFO",
    "SECTION_CODEBOOK",
    "SECTION_CODEBOOK_LENGTH",
    "SCALE_FACTOR_DATA",
    "PULSE_DATA_PRESENT",
    "PULSE_DATA",
    "TNS_DATA_PRESENT",
    "TNS_DATA_N_FILT",
    "TNS_DATA_COEF_RES",
    "TNS_DATA_LENGTH",
    "TNS_DATA_ORDER",
    "TNS_DATA_DIRECTION",
    "TNS_DATA_COEF_COMPRESS",
    "TNS_DATA_COEF",
    "GAIN_CONTROL_DATA_PRESENT",
    "GAIN_CONTROL_DATA",
    "SPECTRAL_DATA"
};

static const size_t maac_ics_state_len_tbl[18] = {
    sizeof("GLOBAL_GAIN") - 1,
    sizeof("ICS_INFO") - 1,
    sizeof("SECTION_CODEBOOK") - 1,
    sizeof("SECTION_CODEBOOK_LENGTH") - 1,
    sizeof("SCALE_FACTOR_DATA") - 1,
    sizeof("PULSE_DATA_PRESENT") - 1,
    sizeof("PULSE_DATA") - 1,
    sizeof("TNS_DATA_PRESENT") - 1,
    sizeof("TNS_DATA_N_FILT") - 1,
    sizeof("TNS_DATA_COEF_RES") - 1,
    sizeof("TNS_DATA_LENGTH") - 1,
    sizeof("TNS_DATA_ORDER") - 1,
    sizeof("TNS_DATA_DIRECTION") - 1,
    sizeof("TNS_DATA_COEF_COMPRESS") - 1,
    sizeof("TNS_DATA_COEF") - 1,
    sizeof("GAIN_CONTROL_DATA_PRESENT") - 1,
    sizeof("GAIN_CONTROL_DATA") - 1,
    sizeof("SPECTRAL_DATA") - 1
};


maac_const
MAAC_PUBLIC
const char*
maac_ics_state_name(const maac_u32 state) {
    return state > MAAC_ICS_STATE_SPECTRAL_DATA ?
      MAAC_ICS_STATE_INVALID_STR
      :
      maac_ics_state_str_tbl[state];
}

maac_const
MAAC_PUBLIC
size_t
maac_ics_state_name_len(const maac_u32 state) {
    return state > MAAC_ICS_STATE_SPECTRAL_DATA ?
      MAAC_ICS_STATE_INVALID_LEN
      :
      maac_ics_state_len_tbl[state];
}

