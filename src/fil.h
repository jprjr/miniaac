/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_FILL_ELEMENT_INCLUDE_GUARD
#define MAAC_FILL_ELEMENT_INCLUDE_GUARD

#include "maac_common.h"
#include "bitreader.h"

enum MAAC_FIL_STATE {
    MAAC_FIL_STATE_COUNT              = 0,
    MAAC_FIL_STATE_ESC_COUNT          = 1,
    MAAC_FIL_STATE_PAYLOAD_TYPE       = 2,
    MAAC_FIL_STATE_PAYLOAD_OTHER_BITS = 3
};

typedef enum MAAC_FIL_STATE MAAC_FIL_STATE;

enum MAAC_FIL_EXT {
    MAAC_FIL_EXT_FILL          = 0x00,
    MAAC_FIL_EXT_FILL_DATA     = 0x01,
    MAAC_FIL_EXT_DYNAMIC_RANGE = 0x0b,
    MAAC_FIL_EXT_SBR_DATA      = 0x0d,
    MAAC_FIL_EXT_SBR_DATA_CRC  = 0x0e
};

typedef enum MAAC_FIL_EXT MAAC_FIL_EXT;

struct maac_fil {
    MAAC_FIL_STATE state;
    maac_u8  extension_type;
    maac_u16 _count;
    maac_u16 _bits;
};

typedef struct maac_fil maac_fil;

MAAC_CDECLS_BEGIN

MAAC_PUBLIC
void
maac_fil_init(maac_fil* f);

MAAC_PUBLIC
MAAC_RESULT
maac_fil_decode(maac_fil* maac_restrict f, maac_bitreader* maac_restrict br);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
