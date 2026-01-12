/* SPDX-License-Identifier: 0BSD */

/* example decoder that uses the maac_raw interface
to interact with individual elements */

#include "maac.h"
#include "maac_extras.h"

#define MAAC_IMPLEMENTATION
#include "maac.h"

#define MAAC_EXTRAS_IMPLEMENTATION
#include "maac_extras.h"

#include "adts_reader.h"
#include "pack.h"
#include "wav.h"
#include "buffer_samples.h"
#include "channel.h"

#ifdef __cplusplus
#include <cstdlib>
#include <cstdio>
#include <cassert>
#else
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#endif

static int SLURPFRAME = 0;
static int DECODEONLY = 0;
static const maac_u32 bitdepth = MAAC_U32_C(16);

struct decoder {
    adts_header header;
    FILE* in;
    FILE* out;
    maac_u32 channels;
    maac_channel* ch;

    maac_u8 *buf; /* our input buffer */
    maac_u32 buflen;
    maac_u8 *outbuf; /* our output buffer of packed samples */

    maac_raw aac;
    maac_bitreader br;
    maac_u32 blocks;
    maac_u32 samples;
    maac_u8 c; /* current channel being output */
    maac_u32 elements[MAAC_RAW_DATA_BLOCK_ID_END + 1]; /* track counts of each element */
};
typedef struct decoder decoder;

static void decoder_init(decoder* d) {
    unsigned int i  = 0;

    d->in = NULL;
    d->out = NULL;
    d->ch = NULL;
    d->buf = NULL;
    d->outbuf = NULL;
    d->buflen = 0;
    d->channels = 0;
    d->blocks = 0;
    d->samples = 0;
    d->c = 0;
    maac_raw_init(&d->aac);
    maac_bitreader_init(&d->br);

    d->br.data = d->buf;

    for(i=0;i<=MAAC_RAW_DATA_BLOCK_ID_END;i++) {
        d->elements[i] = 0;
    }
}

static void decoder_close(decoder* d) {
    if(d->in != NULL) fclose(d->in);
    if(d->out != NULL) fclose(d->out);
    if(d->ch != NULL) free(d->ch);
    if(d->outbuf != NULL) free(d->outbuf);
    free(d);
}

static void decoder_write_samples(decoder* d) {
    maac_u32 c_max = d->c > d->channels ? d->channels : d->c;

    fwrite(d->outbuf, 1, buffer_samples(d->ch, maac_raw_channel_configuration(&d->aac), c_max, d->outbuf) , d->out);

    d->blocks++;
    d->samples += d->ch[0].n_samples;
    d->c = 0;
    return;
}

static int decoder_data_reload(decoder* d) {
    if(fread(d->buf,1,1,d->in) != 1) {
        fprintf(stderr,"Unexpected end-of-file\n");
        return 1;
    }
    d->br.pos = 0;
    d->br.len = 1;
    return 0;
}

static int decoder_raw_sync(decoder* d) {
    MAAC_RESULT res;
    int r;
    while( (res = maac_raw_sync(&d->aac, &d->br)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d)) ) return r;
    }
    if( (r = res != MAAC_OK) ) {
        fprintf(stderr,"received error while decoding raw data block id: %s\n",
          maac_result_name(res));
    }
    return r;
}

static int decoder_raw_sce(decoder* d) {
    MAAC_RESULT res;
    int r;
    maac_channel* ch = d->c < d->channels ? &d->ch[d->c] : NULL;

    while( (res = maac_raw_decode_sce(&d->aac, &d->br, ch)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d)) ) return r;
    }
    if( (r = res != MAAC_OK) ) {
        fprintf(stderr,"received error while decoding single channel element: %s\n",
          maac_result_name(res));
    } else {
        d->c++;
        d->elements[MAAC_RAW_DATA_BLOCK_ID_SCE]++;
    }
    return r;
}

static int decoder_raw_cpe(decoder* d) {
    MAAC_RESULT res;
    int r;
    maac_channel* left  = ((maac_u32)d->c)   < d->channels ? &d->ch[d->c]   : NULL;
    maac_channel* right = ((maac_u32)d->c+1) < d->channels ? &d->ch[d->c+1] : NULL;

    while( (res = maac_raw_decode_cpe(&d->aac, &d->br, left, right)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d)) ) return r;
    }
    if( (r = res != MAAC_OK) ) {
        fprintf(stderr,"received error while decoding channel pair element: %s\n",
          maac_result_name(res));
    } else {
        d->c += 2;
        d->elements[MAAC_RAW_DATA_BLOCK_ID_CPE]++;
    }
    return r;
}

static int decoder_raw_lfe(decoder* d) {
    MAAC_RESULT res;
    int r;
    maac_channel* ch = d->c < d->channels ? &d->ch[d->c] : NULL;

    while( (res = maac_raw_decode_lfe(&d->aac, &d->br, ch)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d)) ) return r;
    }
    if( (r = res != MAAC_OK) ) {
        fprintf(stderr,"received error while decoding low frequency element: %s\n",
          maac_result_name(res));
    } else {
        d->c++;
        d->elements[MAAC_RAW_DATA_BLOCK_ID_LFE]++;
    }
    return r;
}

static int decoder_raw_fil(decoder* d) {
    MAAC_RESULT res;
    int r;

    while( (res = maac_raw_decode_fil(&d->aac, &d->br)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d)) ) return r;
    }
    if( (r = res != MAAC_OK) ) {
        fprintf(stderr,"received error while decoding fill element: %s\n",
          maac_result_name(res));
    } else {
        d->elements[MAAC_RAW_DATA_BLOCK_ID_FIL]++;
    }
    return r;
}

static int decoder_raw_end(decoder* d) {
    d->elements[MAAC_RAW_DATA_BLOCK_ID_END]++;
    decoder_write_samples(d);
    return 0;
}

static int decode_file(const char* infile, const char* outfile) {
    unsigned int i = 0;
    int r = 1;
    decoder *d = NULL;

    d = (decoder*)malloc(sizeof(decoder));
    if(d == NULL) {
        fprintf(stderr,"failed to allocate decoder");
        goto cleanup;
    }
    decoder_init(d);

    d->in = fopen(infile, "rb");
    if(d->in == NULL) {
        fprintf(stderr,"failed to open %s for reading\n", infile);
        goto cleanup;
    }

    if(adts_grab_header(d->in, &d->header, 1) != 0) {
        fprintf(stderr,"failed to grab/parse adts header\n");
        goto cleanup;
    }

    if(maac_raw_config(&d->aac, d->header.aacAudioConfig, sizeof(d->header.aacAudioConfig)) != MAAC_OK) {
        fprintf(stderr,"error in maac_raw_config\n");
        goto cleanup;
    }

    if(!SLURPFRAME) {
        d->buf = (maac_u8*)malloc(sizeof(maac_u8));
        if(d->buf == NULL) {
            fprintf(stderr,"unable to allocate buffer\n");
            goto cleanup;
        }
        d->br.data = d->buf;
        d->br.len = 1;
        d->br.pos = 1;
    }

    d->channels = maac_raw_channels(&d->aac);
    if(!DECODEONLY) {
        d->ch = (maac_channel*)malloc(maac_channel_size() * d->channels);
        if(d->ch == NULL) {
            fprintf(stderr,"failed to allocate aac channels\n");
            goto cleanup;
        }

        d->outbuf = (maac_u8*)malloc(d->channels * 2 * 1024);
        if(d->outbuf == NULL) {
            fprintf(stderr,"failed to allocate output buffer\n");
            goto cleanup;
        }

        d->out = fopen(outfile, "wb");
        if(d->out == NULL) {
            fprintf(stderr,"failed to open %s for writing\n", outfile);
            goto cleanup;
        }

        if(wav_header_create(d->out, maac_raw_sample_rate(&d->aac), d->channels, channel_mask(maac_raw_channel_configuration(&d->aac)), bitdepth) != 0) {
            fprintf(stderr,"error writing out WAV header\n");
            goto cleanup;
        }
    }

    do {
        if(SLURPFRAME) {
            if(d->buflen < d->header.frame_length) {
                if(d->buf == NULL) {
                    d->buf = (maac_u8*)malloc(d->header.frame_length);
                } else {
                    d->buf = (maac_u8*)realloc(d->buf,d->header.frame_length);
                }
                if(d->buf == NULL) {
                    fprintf(stderr,"error allocating/reallocating input buffer\n");
                    goto cleanup;
                }
                d->buflen = d->header.frame_length;
                d->br.data = d->buf;
            }
            if(fread(d->buf,1,d->header.frame_length,d->in) != d->header.frame_length) {
                fprintf(stderr,"out of data while reading ADTS frame\n");
            }
            d->br.pos = 0;
            d->br.len = d->header.frame_length;
        }

        next_element:
        if(decoder_raw_sync(d)) goto cleanup;
        switch(maac_raw_ele_id(&d->aac)) {
            case MAAC_RAW_DATA_BLOCK_ID_SCE: {
                if(decoder_raw_sce(d)) goto cleanup;
                goto next_element;
            }
            case MAAC_RAW_DATA_BLOCK_ID_CPE: {
                if(decoder_raw_cpe(d)) goto cleanup;
                goto next_element;
            }
            case MAAC_RAW_DATA_BLOCK_ID_FIL: {
                if(decoder_raw_fil(d)) goto cleanup;
                goto next_element;
            }
            case MAAC_RAW_DATA_BLOCK_ID_LFE: {
                if(decoder_raw_lfe(d)) goto cleanup;
                goto next_element;
            }
            case MAAC_RAW_DATA_BLOCK_ID_END: {
                if(decoder_raw_end(d)) goto cleanup;
                break;
            }
            default: {
                fprintf(stderr,"Not yet implemented: %s\n", maac_raw_data_block_id_name(maac_raw_ele_id(&d->aac)));
                goto cleanup;
            }
        }
    } while(adts_grab_header(d->in, &d->header, 0) == 0);

    if(!DECODEONLY) wav_header_finish(d->out);

    if(DECODEONLY) {
        fprintf(stderr,"decoded and wrote %u raw data blocks, %u samples\n", d->blocks, d->samples);
    } else {
        fprintf(stderr,"decoded %u raw data blocks, %u samples\n", d->blocks, d->samples);
    }
    fprintf(stderr,"element counts:\n");
    for(i=0; i<= MAAC_RAW_DATA_BLOCK_ID_END; i++) {
        fprintf(stderr,"  %s: %u\n",
          maac_raw_data_block_id_name(i),
          d->elements[i]);
    }
    r = 0;

    cleanup:
    if(d != NULL) decoder_close(d);
    return r;
}



int main(int argc, const char* argv[]) {
    const char* progname = argv[0];
    int minargs = 2;
    argv++;
    argc--;

    while(argc) {
        if(strcmp(*argv,"-n") == 0) {
            argc--;
            argv++;
            minargs--;
            DECODEONLY=1;
        } else if(strcmp(*argv,"-s") == 0) {
            argc--;
            argv++;
            SLURPFRAME=1;
        } else if(strcmp(*argv,"--") == 0) {
            argc--;
            argv++;
            break;
        } else {
            break;
        }
    }

    if(argc < minargs) {
        fprintf(stderr,"Usage: %s [-n] [-s] /path/to/file.aac /path/to/file.wav\n", progname);
        fprintf(stderr,"  Decodes /path/to/file.aac to a wav file, overwrites the destination wav\n");
        fprintf(stderr,"  -s: slurp whole ADTS frames into memory before decoding\n");
        fprintf(stderr,"  -n: decode-only, do not write out WAV files\n");
        return 1;
    }

    return decode_file(argv[0], DECODEONLY ? NULL : argv[1]);
}
