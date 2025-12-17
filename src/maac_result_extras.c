/* SPDX-License-Identifier: 0BSD */

#include "maac_result_extras.h"
#include "maac_result.h"

static const char* MAAC_RESULT_INVALID_STR = "INVALID";
static size_t MAAC_RESULT_INVALID_LEN = sizeof("INVALID") - 1;

static const char* const maac_result_str_tbl[17] = {
    "HUFFMAN_DECODE_ERROR",
    "ADTS_RDB_NOT_CALLED",
    "ADTS_SYNCWORD_NOT_FOUND",
    "PREDICTOR_DATA_NOT_IMPLEMENTED",
    "GAIN_CONTROL_DATA_NOT_IMPLEMENTED",
    "UNSUPPORTED_AOT",
    "PULSE_DATA_NOT_IMPLEMENTED",
    "PCE_NOT_IMPLEMENTED",
    "DSE_NOT_IMPLEMENTED",
    "LFE_NOT_IMPLEMENTED",
    "CCE_NOT_IMPLEMENTED",
    "SF_INDEX_NOT_SET",
    "OUT_OF_SEQUENCE",
    "NOT_IMPLEMENTED",
    "ERROR",
    "CONTINUE",
    "OK"
};

static const size_t maac_result_len_tbl[17] = {
    sizeof("HUFFMAN_DECODE_ERROR") - 1,
    sizeof("ADTS_RDB_NOT_CALLED") - 1,
    sizeof("ADTS_SYNCWORD_NOT_FOUND") - 1,
    sizeof("PREDICTOR_DATA_NOT_IMPLEMENTED") - 1,
    sizeof("GAIN_CONTROL_DATA_NOT_IMPLEMENTED") - 1,
    sizeof("UNSUPPORTED_AOT") - 1,
    sizeof("PULSE_DATA_NOT_IMPLEMENTED") - 1,
    sizeof("PCE_NOT_IMPLEMENTED") - 1,
    sizeof("DSE_NOT_IMPLEMENTED") - 1,
    sizeof("LFE_NOT_IMPLEMENTED") - 1,
    sizeof("CCE_NOT_IMPLEMENTED") - 1,
    sizeof("SF_INDEX_NOT_SET") - 1,
    sizeof("OUT_OF_SEQUENCE") - 1,
    sizeof("NOT_IMPLEMENTED") - 1,
    sizeof("ERROR") - 1,
    sizeof("CONTINUE") - 1,
    sizeof("OK") - 1
};


maac_const
MAAC_PUBLIC
const char*
maac_result_name(const maac_s32 result) {
    return result > MAAC_RESULT_MAX ?
      MAAC_RESULT_INVALID_STR
      :
        result < MAAC_RESULT_MIN ?
          MAAC_RESULT_INVALID_STR :
          maac_result_str_tbl[15 + result];
}

maac_const
MAAC_PUBLIC
size_t
maac_result_name_len(const maac_s32 result) {
    return result > MAAC_RESULT_MAX ?
      MAAC_RESULT_INVALID_LEN
      :
        result < MAAC_RESULT_MIN ?
          MAAC_RESULT_INVALID_LEN :
          maac_result_len_tbl[15 + result];
}
