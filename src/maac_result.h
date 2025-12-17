/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_RESULT_INCLUDE_GUARD
#define MAAC_RESULT_INCLUDE_GUARD

#include "maac_stdint.h"

#define MAAC_UNREACHABLE                       (-99) /* strictly used in MAAC_UNREACHABLE_RETURN */
#define MAAC_HUFFMAN_DECODE_ERROR              (-15)
#define MAAC_ADTS_RDB_NOT_CALLED               (-14)
#define MAAC_ADTS_SYNCWORD_NOT_FOUND           (-13)
#define MAAC_PREDICTOR_DATA_NOT_IMPLEMENTED    (-12)
#define MAAC_GAIN_CONTROL_DATA_NOT_IMPLEMENTED (-11)
#define MAAC_UNSUPPORTED_AOT                   (-10)
#define MAAC_PULSE_DATA_NOT_IMPLEMENTED        ( -9)
#define MAAC_PCE_NOT_IMPLEMENTED               ( -8)
#define MAAC_DSE_NOT_IMPLEMENTED               ( -7)
#define MAAC_LFE_NOT_IMPLEMENTED               ( -6)
#define MAAC_CCE_NOT_IMPLEMENTED               ( -5)
#define MAAC_SF_INDEX_NOT_SET                  ( -4)
#define MAAC_OUT_OF_SEQUENCE                   ( -3)
#define MAAC_NOT_IMPLEMENTED                   ( -2)
#define MAAC_ERROR                             ( -1) /* generic error, likely in an invalid state */
#define MAAC_CONTINUE                          (  0) /* needs more data, otherwise fine */
#define MAAC_OK                                (  1) /* generic "OK" */

#define MAAC_RESULT_MIN MAAC_HUFFMAN_DECODE_ERROR
#define MAAC_RESULT_MAX MAAC_OK

typedef maac_s32 MAAC_RESULT;

#endif /* INCLUDE_GUARD */
