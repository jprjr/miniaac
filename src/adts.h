/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_ADTS_INCLUDE_GUARD
#define MAAC_ADTS_INCLUDE_GUARD

#include "maac_common.h"
#include "bitreader.h"
#include "bitfield.h"

#include "raw.h"

/*

This is a decoder for ADTS-encapsulated AAC data.

Setup is roughly:

* call maac_adts_init()
* call maac_adts_sync() to read the first header
* Assign output channel objects

Then to decode:
* Call maac_adts_decode() until it returns MAAC_OK
* Do things with your samples, repeat.

It will automatically track which audio channel
should receive output, and handle multiple data
blocks if your ADTS has multiple data blocks (this
is very rare).

If you want to manage manage individual elements
you can do that too.

After a call to call maac_adts_sync() - you can
call maac_adts_raw_sync(), then inspect the
raw data block's element id (raw.ele_id).

Then call one of the following:
  - maac_adts_decode_fil()
  - maac_adts_decode_sce()
  - maac_adts_decode_cpe()
* Repeat  until raw.ele_id is MAAC_RAW_DATA_BLOCK_ID_END
* If you have more then 1 raw data block, you'll need to
  repeat this process again (and track how many raw data
  blocks you've handled).
* Once you've handled all raw data blocks, you need
  to manually set the state back to MAAC_ADTS_STATE_SYNCWORD
  and then call the next maac_adts_sync().
*/


enum MAAC_ADTS_STATE {
    MAAC_ADTS_STATE_SYNCWORD,
    MAAC_ADTS_STATE_VERSION,
    MAAC_ADTS_STATE_LAYER,
    MAAC_ADTS_STATE_PROTECTION,
    MAAC_ADTS_STATE_PROFILE,
    MAAC_ADTS_STATE_FREQ_INDEX,
    MAAC_ADTS_STATE_PRIVATE,
    MAAC_ADTS_STATE_CHANNEL_INDEX,
    MAAC_ADTS_STATE_ORIG,
    MAAC_ADTS_STATE_HOME,
    MAAC_ADTS_STATE_COPYRIGHT,
    MAAC_ADTS_STATE_COPYRIGHT_START,
    MAAC_ADTS_STATE_LENGTH,
    MAAC_ADTS_STATE_BUFFER_FULLNESS,
    MAAC_ADTS_STATE_RAW_DATA_BLOCKS,
    MAAC_ADTS_STATE_CRC16,
    MAAC_ADTS_STATE_RAW_DATA_BLOCK_POSITION,
    MAAC_ADTS_STATE_RAW_DATA_BLOCK,
    MAAC_ADTS_STATE_RAW_DATA_BLOCK_CRC16
};

typedef enum MAAC_ADTS_STATE MAAC_ADTS_STATE;

struct maac_adts {
    MAAC_ADTS_STATE state;

    /* this should all pack into a single 32-bit int */
    struct {
        MAAC_BITFIELD(maac_u16,frame_length,13);
        MAAC_BITFIELD(maac_u16,buffer_fullness,11);
        MAAC_BITFIELD(maac_u8,copyright_id_bit,1);
        MAAC_BITFIELD(maac_u8,copyright_id_start,1);
        MAAC_BITFIELD(maac_u8,raw_data_blocks,2);
    } variable_header;

    struct {
        MAAC_BITFIELD(maac_u8, version, 1);
        MAAC_BITFIELD(maac_u8, layer, 2);
        MAAC_BITFIELD(maac_u8, protection_absent, 1);
        MAAC_BITFIELD(maac_u8, profile, 2);
        MAAC_BITFIELD(maac_u8, sampling_frequency_index, 4);
        MAAC_BITFIELD(maac_u8, channel_configuration, 3);
        MAAC_BITFIELD(maac_u8, original_copy, 1);
        MAAC_BITFIELD(maac_u8, home, 1);
    } fixed_header;

    /* used for both adts_error_check and adts_header_error_check */
    maac_u16 crc;
    maac_u16 raw_data_block_position[3];

    /* how many bytes we're willing to discard while finding
       the first syncword, defaults to 0 */
    maac_u32 tolerance;

    maac_raw raw;

    maac_u32 _i;
};

typedef struct maac_adts maac_adts;

MAAC_CDECLS_BEGIN

MAAC_PUBLIC
void
maac_adts_init(maac_adts* a);

/* returns MAAC_OK just before starting to decode the next raw data block.
so you can inspect ADTS frame headers for info like sample rate, channels, etc.
   returns MAAC_CONTINUE if more data is needed (so you'll need to refill the
   bitreader) */
MAAC_PUBLIC
MAAC_RESULT
maac_adts_sync(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br);

/* returns MAAC_OK just after decoding a raw data block. Automatically
calls maac_adts_sync() if needed. */
MAAC_PUBLIC
MAAC_RESULT
maac_adts_decode(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br);

/* lower-level API - after a successful call to maac_adts_sync(),
use this to read the next element id in the raw data block */
MAAC_PUBLIC
MAAC_RESULT
maac_adts_raw_sync(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br);

/* lower-level API - after a call to maac_adts_raw_sync(), decode a FIL element */
MAAC_PUBLIC
MAAC_RESULT
maac_adts_raw_decode_fil(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br);

/* lower-level API - after a call to maac_adts_raw_sync(), decode a SCE element */
MAAC_PUBLIC
MAAC_RESULT
maac_adts_raw_decode_sce(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br, maac_channel* maac_restrict  c);

/* lower-level API - after a call to maac_adts_raw_sync(), decode a CPE element */
MAAC_PUBLIC
MAAC_RESULT
maac_adts_raw_decode_cpe(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br, maac_channel* maac_restrict left, maac_channel* maac_restrict right);

/* lower-level API - after a call to maac_adts_raw_sync(), decode a LFE element */
MAAC_PUBLIC
MAAC_RESULT
maac_adts_raw_decode_lfe(maac_adts* maac_restrict a, maac_bitreader* maac_restrict br, maac_channel* maac_restrict  c);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
