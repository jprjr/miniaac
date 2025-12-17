/* SPDX-License-Identifier: 0BSD */
#include "maac.h"
#include "maac_extras.h"

#define MAAC_IMPLEMENTATION
#include "maac.h"

#define MAAC_EXTRAS_IMPLEMENTATION
#include "maac_extras.h"

#include "adts_reader.h"
#include "pack.h"
#include "wav.h"

#ifdef __cplusplus
#include <cstdlib>
#include <cstdio>
#else
#include <stdlib.h>
#include <stdio.h>
#endif

static const maac_u32 bitdepth = MAAC_U32_C(16);

struct decoder {
    adts_header header;
    FILE* in;
    FILE* out;
    maac_u32 channels;
    maac_channel* ch;

    maac_u8 buf[1]; /* out single-byte data buffer */
    maac_u8 *outbuf; /* our output buffer of packed samples */

    maac_raw aac;
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
    maac_raw_init(&d->aac);
    maac_bitreader_init(&d->br);

    d->br.data = d->buf;
}

static void decoder_close(decoder* d) {
    if(d->in != NULL) fclose(d->in);
    if(d->out != NULL) fclose(d->out);
    if(d->ch != NULL) free(d->ch);
    if(d->outbuf != NULL) free(d->outbuf);
    free(d);
}

static void decoder_write_samples(decoder* d) {
    maac_u32 c;
    maac_u32 i;
    maac_s32 s;
    for(c=0;c<d->channels;c++) {
        for(i=0;i<d->ch[c].n_samples;i++) {
            s = (maac_s32)d->ch[c].samples[i];
            s = maac_clamp(s, MAAC_S16_MIN, MAAC_S16_MAX);
            pack_s16le(&d->outbuf[ ((i * d->channels) * 2) + (c*2)], (maac_s16)s);
        }
    }
    fwrite(d->outbuf, 1, d->channels * 2 * d->ch[0].n_samples, d->out);
    d->frames++;
    return;
}

static int decode_file(const char* infile, const char* outfile) {
    MAAC_RESULT res;
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

    if(adts_grab_header(d->in, &d->header) != 0) {
        fprintf(stderr,"failed to grab/parse adts header\n");
        goto cleanup;
    }

    if(maac_raw_config(&d->aac, d->header.aacAudioConfig, sizeof(d->header.aacAudioConfig)) != MAAC_OK) {
        fprintf(stderr,"error in maac_raw_config\n");
        goto cleanup;
    }

    d->channels = maac_raw_channels(&d->aac);
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

    if(wav_header_create(d->out, maac_raw_sample_rate(&d->aac), d->channels, bitdepth) != 0) {
        fprintf(stderr,"error writing out WAV header\n");
        goto cleanup;
    }

    maac_raw_set_out_channels(&d->aac, d->ch);
    maac_raw_set_num_out_channels(&d->aac, d->channels);

    do {
        while( (res = maac_raw_decode(&d->aac, &d->br)) == MAAC_CONTINUE) {
            if(fread(d->buf,1,1,d->in) != 1) {
                fprintf(stderr,"out of data while decoding raw data block id\n");
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
        decoder_write_samples(d);
    } while(adts_grab_header(d->in, &d->header) == 0);
    wav_header_finish(d->out, bitdepth);

    fprintf(stderr,"decoded and wrote %u frames\n", d->frames);
    r = 0;

    cleanup:
    if(d != NULL) decoder_close(d);
    return r;
}



int main(int argc, const char* argv[]) {
    const char* progname = argv[0];
    argv++;
    argc--;

    if(argc < 2) {
        fprintf(stderr,"Usage: %s /path/to/file.aac /path/to/file.wav\n", progname);
        fprintf(stderr,"  Decodes /path/to/file.aac to a wav file, overwrites the destination wav\n");
        return 1;
    }

    return decode_file(argv[0], argv[1]);
}
