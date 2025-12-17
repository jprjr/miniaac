/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_PULSE_DATA_INCLUDE_GUARD
#define MAAC_PULSE_DATA_INCLUDE_GUARD

#include "maac_common.h"
#include "maac_inline.h"
#include "bitreader.h"

enum MAAC_PULSE_STATE {
    MAAC_PULSE_STATE_NUMBER    = 0,
    MAAC_PULSE_STATE_START_SFB = 1,
    MAAC_PULSE_STATE_OFFSET    = 2,
    MAAC_PULSE_STATE_AMP       = 3
};

typedef enum MAAC_PULSE_STATE MAAC_PULSE_STATE;

struct maac_pulse {
    MAAC_PULSE_STATE state;
    maac_u8 num_pulse;
    maac_u8 start_sfb;
    maac_u16 pulses[4]; /* packed 5 bits offset, 4 bits amp */
    maac_u8 _n;
};

typedef struct maac_pulse maac_pulse;

MAAC_PRIVATE
void
maac_pulse_init(maac_pulse* p);

MAAC_PRIVATE
MAAC_RESULT
maac_pulse_parse(maac_pulse* maac_restrict p, maac_bitreader* maac_restrict br);

#endif /* INCLUDE_GUARD */


