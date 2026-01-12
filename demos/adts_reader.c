/* SPDX-License-Identifier: 0BSD */
#include "adts_reader.h"

int adts_grab_header(FILE* input, adts_header* header, int fatal) {
    maac_u8 buffer[7]; /* we only read the "main" 7 bytes of header, if there's CRC we discard it */
    maac_u16 audioConfig;

    if(fread(buffer,1,7,input) != 7) {
        /* fatal is 1 on the first read - after the first read we don't complain */
        if(fatal) fprintf(stderr,"ADTS: unable to read minimum header\n");
        return -1;
    }

    header->syncword = (((uint16_t)buffer[0]) << 4) | (((uint16_t)buffer[1]) >> 4);
    if(header->syncword != 0x0FFF) {
        fprintf(stderr,"ADTS: invalid syncword\n");
        return -1;
    }

    header->version =  (buffer[1] & 0x08) >> 3;
    header->layer =    (buffer[1] & 0x06) >> 1;
    header->no_crc =   (buffer[1] & 0x01);
    header->profile =  (buffer[2] & 0xC0) >> 6;
    header->freq_index =  (buffer[2] & 0x3C) >> 2;
    header->priv  =  (buffer[2] & 0x02) >> 1;
    header->ch_config = ((buffer[2] & 0x01) << 2) | ((buffer[3] & 0xC0) >> 6);
    header->original = (buffer[3] & 0x20) >> 5;
    header->home = (buffer[3] & 0x10) >> 4;
    header->copyright_bit = (buffer[3] & 0x08) >> 3;
    header->copyright_start = (buffer[3] & 0x04) >> 2;
    header->frame_length = ((buffer[3] & 0x02) << 11) | ((buffer[4] << 3)) | ((buffer[5] & 0xE0) >> 5);
    header->buffer_fullness = ((buffer[5] & 0x1F) << 2) | ((buffer[6] & 0xFC) >> 2);
    header->frames = buffer[6] & 0x03;
    if(header->frames) {
        fprintf(stderr,"ADTS: this demo only supports single-rdb ADTS frames\n");
        return -1;
    }

    if(header->no_crc == 0) {
        if(fread(buffer,1,2,input) != 2) {
            fprintf(stderr,"ADTS: unable to read header CRC\n");
            return -1;
        }
        header->crc = buffer[0] << 8 | buffer[1];
        header->frame_length -= 9;
    } else {
        header->frame_length -= 7;
        header->crc = 0;
    }

    audioConfig = (0x02 << 11) | (header->freq_index << 7) | (header->ch_config << 3) | 0;
    header->aacAudioConfig[0] = (audioConfig >> 8) & 0xFF;
    header->aacAudioConfig[1] = (audioConfig >> 0) & 0xFF;

    return 0;
}

