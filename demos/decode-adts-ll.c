/* SPDX-License-Identifier: 0BSD */

/* example ADTS decoder, that uses the lower-level
interfaces to interact with individual elements. */

#include "maac.h"
#include "maac_extras.h"

#define MAAC_IMPLEMENTATION
#include "maac.h"

#define MAAC_EXTRAS_IMPLEMENTATION
#include "maac_extras.h"

#include "pack.h"
#include "wav.h"
#include "buffer_samples.h"
#include "channel.h"

#ifdef __cplusplus
#include <cstdlib>
#include <cstdio>
#include <cstring>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

static int SLURPFILE = 0;
static int DECODEONLY = 0;
static const maac_u32 bitdepth = MAAC_U32_C(16);

struct decoder {
    FILE* in;
    FILE* out;
    maac_u32 channels;
    maac_channel* ch;

    maac_u8 *buf; /* our data buffer */

    maac_u8 *outbuf; /* our output buffer of packed samples */

    maac_adts adts;
    maac_bitreader br;
    maac_u32 frames;
    maac_u32 blocks; /* total raw data blocks */
    maac_u32 samples;
    maac_u8 c; /* current channel being output */
    maac_u8 rdb; /* current raw data block being processed */
    maac_u32 elements[MAAC_RAW_DATA_BLOCK_ID_END + 1]; /* track counts of each element */
};
typedef struct decoder decoder;

static void decoder_init(decoder* d) {
    unsigned int i  = 0;
    d->in = NULL;
    d->out = NULL;
    d->ch = NULL;
    d->outbuf = NULL;
    d->channels = 0;
    d->frames = 0;
    d->blocks = 0;
    d->samples = 0;
    d->c = 0;
    d->rdb = 0;
    maac_adts_init(&d->adts);
    maac_bitreader_init(&d->br);

    for(i=0;i<=MAAC_RAW_DATA_BLOCK_ID_END;i++) {
        d->elements[i] = 0;
    }
}

static void decoder_close(decoder* d) {
    if(d->in != NULL) fclose(d->in);
    if(d->out != NULL) fclose(d->out);
    if(d->ch != NULL) free(d->ch);
    if(d->buf != NULL) free(d->buf);
    if(d->outbuf != NULL) free(d->outbuf);
    free(d);
}

static void decoder_write_samples(decoder* d) {
    maac_u32 c_max = d->c > d->channels ? d->channels : d->c;

    fwrite(d->outbuf, 1, buffer_samples(d->ch, maac_adts_channel_configuration(&d->adts), c_max, d->outbuf) , d->out);

    d->blocks++;
    d->samples += d->ch[0].n_samples;
    d->c = 0;
    return;
}

static int decoder_data_reload(decoder* d, int fatal) {
    if(fread(d->buf,1,1,d->in) != 1) {
        if(fatal) fprintf(stderr,"Unexpected end-of-file\n");
        return 1;
    }
    d->br.pos = 0;
    d->br.len = 1;
    return 0;
}

static int decoder_adts_sync(decoder* d) {
    MAAC_RESULT res;
    int r;
    while( (res = maac_adts_sync(&d->adts, &d->br)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d,0)) ) return r;
    }
    if( (r = res != MAAC_OK) ) {
        fprintf(stderr,"received error while decoding raw data block element id: %s\n",
          maac_result_name(res));
    }
    return r;
}

static int decoder_adts_raw_sync(decoder* d) {
    MAAC_RESULT res;
    int r;
    while( (res = maac_adts_raw_sync(&d->adts, &d->br)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d,1)) ) return r;
    }
    if( (r = res != MAAC_OK) ) {
        fprintf(stderr,"received error while decoding raw data block element id: %s\n",
          maac_result_name(res));
    }
    return r;
}

static int decoder_adts_raw_sce(decoder* d) {
    MAAC_RESULT res;
    int r;
    maac_channel* ch = d->c < d->channels ? &d->ch[d->c] : NULL;

    while( (res = maac_adts_raw_decode_sce(&d->adts, &d->br, ch)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d,1)) ) return r;
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

static int decoder_adts_raw_cpe(decoder* d) {
    MAAC_RESULT res;
    int r;
    maac_channel* left  = ((maac_u32)d->c)   < d->channels ? &d->ch[d->c]   : NULL;
    maac_channel* right = ((maac_u32)d->c+1) < d->channels ? &d->ch[d->c+1] : NULL;

    while( (res = maac_adts_raw_decode_cpe(&d->adts, &d->br, left, right)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d,1)) ) return r;
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

static int decoder_adts_raw_lfe(decoder* d) {
    MAAC_RESULT res;
    int r;
    maac_channel* ch = d->c < d->channels ? &d->ch[d->c] : NULL;

    while( (res = maac_adts_raw_decode_lfe(&d->adts, &d->br, ch)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d,1)) ) return r;
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

static int decoder_adts_raw_fil(decoder* d) {
    MAAC_RESULT res;
    int r;

    while( (res = maac_adts_raw_decode_fil(&d->adts, &d->br)) == MAAC_CONTINUE) {
        if( (r = decoder_data_reload(d,1)) ) return r;
    }
    if( (r = res != MAAC_OK) ) {
        fprintf(stderr,"received error while decoding fill element: %s\n",
          maac_result_name(res));
    } else {
        d->elements[MAAC_RAW_DATA_BLOCK_ID_FIL]++;
    }
    return r;
}

static int decoder_adts_raw_end(decoder* d) {
    d->elements[MAAC_RAW_DATA_BLOCK_ID_END]++;
    if(!DECODEONLY) decoder_write_samples(d);
    return 0;
}

static int decode_file(const char* infile, const char* outfile) {
    MAAC_RESULT res;
    maac_u32 i;
    int r = 1;
    decoder *d = NULL;
    long l;

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

    if(SLURPFILE) {
        if(fseek(d->in, 0, SEEK_END) != 0) {
            fprintf(stderr,"error seeking to end of file\n");
            goto cleanup;
        }
        if( (l = ftell(d->in)) <= 0) {
            fprintf(stderr,"error getting lenfth of file\n");
            goto cleanup;
        }
        if(fseek(d->in, 0, SEEK_SET) != 0) {
            fprintf(stderr,"error seeking to start of file\n");
            goto cleanup;
        }
        d->buf = (maac_u8*)malloc((size_t)l);
        if(d->buf == NULL) {
            fprintf(stderr,"error allocating file buffer\n");
            goto cleanup;
        }
        if(fread(d->buf,1,(size_t)l,d->in) != (size_t)l) {
            fprintf(stderr,"error reading full file\n");
            goto cleanup;
        }
        d->br.data = d->buf;
        d->br.len = (maac_u32)l;
        d->br.pos = 0;
    } else {
        d->buf = (maac_u8*)malloc(sizeof(maac_u8));
        if(d->buf == NULL) {
            fprintf(stderr,"error allocating file buffer\n");
            goto cleanup;
        }
        d->br.data = d->buf;
        d->br.len = 1;
        d->br.pos = 1;
    }

    while( (res = maac_adts_sync(&d->adts, &d->br)) == MAAC_CONTINUE) {
        if(fread(d->buf,1,1,d->in) != 1) {
            fprintf(stderr,"out of data while looking for first ADTS header\n");
            goto cleanup;
        }
        d->br.pos = 0;
        d->br.len = 1;
    }

    d->channels = maac_adts_channels(&d->adts);
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

        if(wav_header_create(d->out, maac_adts_sample_rate(&d->adts), d->channels, channel_mask(maac_adts_channel_configuration(&d->adts)), bitdepth) != 0) {
            fprintf(stderr,"error writing out WAV header\n");
            goto cleanup;
        }

        maac_adts_set_out_channels(&d->adts, d->ch);
        maac_adts_set_num_out_channels(&d->adts, d->channels);
    }

    do {
        while(d->rdb < maac_adts_raw_data_blocks(&d->adts)) {
            next_element:
            if(decoder_adts_raw_sync(d)) goto cleanup;
            switch(maac_adts_raw_ele_id(&d->adts)) {
                case MAAC_RAW_DATA_BLOCK_ID_SCE: {
                    if(decoder_adts_raw_sce(d)) goto cleanup;
                    goto next_element;
                }
                case MAAC_RAW_DATA_BLOCK_ID_CPE: {
                    if(decoder_adts_raw_cpe(d)) goto cleanup;
                    goto next_element;
                }
                case MAAC_RAW_DATA_BLOCK_ID_FIL: {
                    if(decoder_adts_raw_fil(d)) goto cleanup;
                    goto next_element;
                }
                case MAAC_RAW_DATA_BLOCK_ID_LFE: {
                    if(decoder_adts_raw_lfe(d)) goto cleanup;
                    goto next_element;
                }
                case MAAC_RAW_DATA_BLOCK_ID_END: {
                    if(decoder_adts_raw_end(d)) goto cleanup;
                    break;
                }
                default: {
                    fprintf(stderr,"Not yet implemented: %s\n", maac_raw_data_block_id_name(maac_adts_raw_ele_id(&d->adts)));
                    goto cleanup;
                }
            }
            d->rdb++;
        }
        d->rdb = 0;
        d->frames++;

        if(d->br.pos == d->br.len && d->br.len == 1) {
            if(fread(d->buf,1,1,d->in) == 1) {
                d->br.pos = 0;
                d->br.len = 1;
            }
        }
    } while(decoder_adts_sync(d) == 0);
    if(!DECODEONLY) wav_header_finish(d->out);

    if(DECODEONLY) {
        fprintf(stderr,"decoded %u ADTS frames, %u raw data blocks\n", d->frames, d->blocks);
    } else {
        fprintf(stderr,"decoded and wrote %u ADTS frames, %u raw data blocks\n", d->frames, d->blocks);
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
            SLURPFILE=1;
        } else if(strcmp(*argv,"--") == 0) {
            argc--;
            argv++;
            break;
        } else {
            break;
        }
    }

    if( argc < minargs) {
        fprintf(stderr,"Usage: %s [-n] [-s] /path/to/file.aac /path/to/file.wav\n", progname);
        fprintf(stderr,"  Decodes /path/to/file.aac to a wav file, overwrites the destination wav\n");
        fprintf(stderr,"  -n: disable writing audio, only decode\n");
        fprintf(stderr,"  -s: read the entire aac file into memory\n");
        return 1;
    }

    return decode_file(argv[0], DECODEONLY ? NULL : argv[1]);
}
