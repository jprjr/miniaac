/* SPDX-License-Identifier: 0BSD */
#include "maac_consts_extras.h"
#include "maac_consts.h"

static const char* MAAC_WINDOW_SEQUENCE_INVALID = "INVALID";
size_t MAAC_WINDOW_SEQUENCE_INVALID_LEN = sizeof("INVALID") - 1;

static const char* const maac_window_sequence_str_tbl[4] = {
    "ONLY_LONG",
    "LONG_START",
    "EIGHT_SHORT",
    "LONG_STOP"
};

static const size_t maac_window_sequence_len_tbl[4] = {
    sizeof("ONLY_LONG") - 1,
    sizeof("LONG_START") - 1,
    sizeof("EIGHT_SHORT") - 1,
    sizeof("LONG_STOP") - 1
};

maac_const
MAAC_PUBLIC
const char*
maac_window_sequence_name(const maac_u32 seq) {
    return seq > MAAC_WINDOW_SEQUENCE_LONG_STOP ?
      MAAC_WINDOW_SEQUENCE_INVALID
      :
      maac_window_sequence_str_tbl[seq];
}

maac_const
MAAC_PUBLIC
size_t
maac_window_sequence_name_len(const maac_u32 seq) {
    return seq > MAAC_WINDOW_SEQUENCE_LONG_STOP ?
      MAAC_WINDOW_SEQUENCE_INVALID_LEN
      :
      maac_window_sequence_len_tbl[seq];
}

