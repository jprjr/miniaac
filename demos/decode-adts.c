/* SPDX-License-Identifier: 0BSD */

/* Example program for decoding via maac_adts,
using just the maac_adts_decode() function */

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
};
typedef struct decoder decoder;

static void decoder_init(decoder* d) {
    d->in = NULL;
    d->out = NULL;
    d->ch = NULL;
    d->outbuf = NULL;
    d->channels = 0;
    d->frames = 0;
    maac_adts_init(&d->adts);
    maac_bitreader_init(&d->br);
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
    fwrite(d->outbuf, 1, buffer_samples(d->ch, maac_adts_channel_configuration(&d->adts), d->channels, d->outbuf) , d->out);
    return;
}

static int decode_file(const char* infile, const char* outfile) {
    MAAC_RESULT res;
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
        while( (res = maac_adts_decode(&d->adts, &d->br)) == MAAC_CONTINUE) {
            if(fread(d->buf,1,1,d->in) != 1) {
                fprintf(stderr,"out of data while decoding adts block\n");
                goto cleanup;
            }
            d->br.pos = 0;
            d->br.len = 1;
        }
        if(res != MAAC_OK) {
            fprintf(stderr,"received error while decoding raw data block id: %s\n",
              maac_result_name(res));
            goto cleanup;
        }
        d->frames++;
        if(!DECODEONLY) decoder_write_samples(d);

        if(d->br.pos == d->br.len && d->br.len == 1) {
            if(fread(d->buf,1,1,d->in) == 1) {
                d->br.pos = 0;
                d->br.len = 1;
            }
        }
    } while(d->br.pos < d->br.len);
    if(!DECODEONLY) wav_header_finish(d->out);

    if(DECODEONLY) {
        fprintf(stderr,"decoded %u frames\n", d->frames);
    } else {
        fprintf(stderr,"decoded and wrote %u frames\n", d->frames);
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
