/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_RAW_INCLUDE_GUARD
#define MAAC_RAW_INCLUDE_GUARD

#include "maac_common.h"
#include "bitreader.h"

#include "channel.h"
#include "fil.h"
#include "sce.h"
#include "cpe.h"

/*

This is our main decoder for raw AAC data.

Setup is roughly:

* Call maac_raw_init().
* Configure the sampling frequency index (sf_index).
  - this can be done with maac_raw_config(), which
    expects to receive a full AudioSpecifcConfig
* Assign output channels (channels, num_channels)

Then to decode:

* Call maac_raw_decode() until it returns MAAC_OK
* Do things with your samples, repeat.

It will automatically track which audio channel should
receive output and handle it appropriately.

There are lower-level APIs too, in case you want
to involve yourself with managing individual channel
elements.

Setup is:

* Call maac_raw_init()
* Configure the sampling frequency index (sf_index).

Then to decode:

* Call maac_raw_sync() until it returns MAAC_OK
* Inspect the current element ID (ele_id)
  Call one of the following, based on element ID:
  - maac_raw_decode_fil()
  - maac_raw_decode_sce()
  - maac_raw_decode_cpe()
* Repeat the maac_raw_sync + maac_raw_decode* combo until
  ele_id is MAAC_RAW_DATA_BLOCK_ID_END
  Handle any post-decode steps you need to, repeat.

After a decode call you can retrieve the element's struct if
you wish to inspect further:
  raw.ele.fil, raw.ele.sce, raw.ele.cpe

*/

enum MAAC_RAW_DATA_BLOCK_ID {
    MAAC_RAW_DATA_BLOCK_ID_SCE = 0x00,
    MAAC_RAW_DATA_BLOCK_ID_CPE = 0x01,
    MAAC_RAW_DATA_BLOCK_ID_CCE = 0x02,
    MAAC_RAW_DATA_BLOCK_ID_LFE = 0x03,
    MAAC_RAW_DATA_BLOCK_ID_DSE = 0x04,
    MAAC_RAW_DATA_BLOCK_ID_PCE = 0x05,
    MAAC_RAW_DATA_BLOCK_ID_FIL = 0x06,
    MAAC_RAW_DATA_BLOCK_ID_END = 0x07
};

typedef enum MAAC_RAW_DATA_BLOCK_ID MAAC_RAW_DATA_BLOCK_ID;

enum MAAC_RAW_STATE {
    MAAC_RAW_STATE_BLOCK_ID = 0,
    MAAC_RAW_STATE_FIL      = 1,
    MAAC_RAW_STATE_SCE      = 2,
    MAAC_RAW_STATE_CPE      = 3
};

typedef enum MAAC_RAW_STATE MAAC_RAW_STATE;

union maac_raw_element {
    maac_fil fil;
    maac_sce sce;
    maac_cpe cpe;
};

typedef union maac_raw_element maac_raw_element;

struct maac_raw {
    /* these are intended to be inspected/set by the user */

    /* our current sync/decode state */
    MAAC_RAW_STATE state;

    /* the sampling frequency index */
    maac_u8 sf_index;

    /* the current element ID (after a call to sync()) */
    maac_u8 ele_id;

    /* Storage for the element being decoded */
    maac_raw_element ele;

    /* output audio channels */
    maac_channel* out_channels;
    maac_u8 num_out_channels;

    /* these are intended to be read-only fields (not set by user) */
    maac_u32 sample_rate;
    maac_u8 channel_configuration;

    /* these are intended to be internal fields */
    maac_u32 rand_state;
    maac_u8 _c; /* current channel being decoded to */
};

typedef struct maac_raw maac_raw;

MAAC_CDECLS_BEGIN

MAAC_PUBLIC
void
maac_raw_init(maac_raw* r);

/* instead of manually configuring the sf_index, you can
use this function with a buffer of AudioSpecificConfig */
MAAC_PUBLIC
MAAC_RESULT
maac_raw_config(maac_raw* r, const maac_u8* data, maac_u32 len);

/* the more "managed" API - returns MAAC_OK after decoding the full
raw data block */
MAAC_PUBLIC
MAAC_RESULT
maac_raw_decode(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br);

/* the lower-level API - returns MAAC_OK after decoding the next element ID */
MAAC_PUBLIC
MAAC_RESULT
maac_raw_sync(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br);

/* Returns MAAC_OK after decoding a FIL element */
MAAC_PUBLIC
MAAC_RESULT
maac_raw_decode_fil(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br);

/* Returns MAAC_OK after decoding a SCE element */
MAAC_PUBLIC
MAAC_RESULT
maac_raw_decode_sce(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br, maac_channel* maac_restrict c);

/* Returns MAAC_OK after decoding a CPE element */
MAAC_PUBLIC
MAAC_RESULT
maac_raw_decode_cpe(maac_raw* maac_restrict r, maac_bitreader* maac_restrict br, maac_channel* maac_restrict left, maac_channel* maac_restrict right);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
