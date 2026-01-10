/* SPDX-License-Identifier: 0BSD */
#include "filterbank.h"

#include "maac_memcpy.h"
#include "maac_consts.h"
#include "imdct.h"
#include "imdct_window_data.h"
#include "maac_assert.h"

MAAC_PRIVATE
void
maac_filterbank(maac_flt* samples, maac_flt* overlap, const maac_filterbank_params* p) {
    maac_u16 i = 0;
    const maac_flt* window = NULL;
    const maac_flt* window_prev = NULL;

    const maac_u16 long_len = 1024;
    const maac_u16 short_len = long_len / 8;

    const maac_u16 mid_len = (long_len - short_len) / 2; /* 448 */
    const maac_u16 trans_len = short_len/2; /* 64 */

    maac_u8 wseq = p->window_sequence;
    maac_u8 window_shape = p->window_shape;
    maac_u8 window_shape_prev = p->window_shape_prev;

    switch(wseq) {
        case MAAC_WINDOW_SEQUENCE_ONLY_LONG: {
            maac_imdct(samples, long_len*2);

            window = window_shape == 1 ? maac_window_kbd_1024 : maac_window_sin_1024;
            window_prev = window_shape_prev == 1 ? maac_window_kbd_1024 : maac_window_sin_1024;

            /* add windowed overlap from previous frame into output of this frame */
            for(i=0;i<long_len;i++) {
                samples[i] = overlap[i] + samples[i] * window_prev[i];
            }

            /* copy windowed imdct output to overlap */
            for(i=0;i<long_len;i++) {
                overlap[i] = samples[i+long_len] * window[long_len - 1 - i];
            }
            break;
        }

        case MAAC_WINDOW_SEQUENCE_LONG_START: {
            maac_imdct(samples, long_len*2);

            /* this is something of a transitional sequence, it goes
               between an ONLY_LONG and an EIGHT_SHORT, so our current window
               is a short window */
            window = window_shape == 1 ? maac_window_kbd_128 : maac_window_sin_128;
            window_prev = window_shape == 1 ? maac_window_kbd_1024 : maac_window_sin_1024;

            /* add windowed overlap from previous long frame to our output */
            for(i=0;i<long_len;i++) {
                samples[i] = overlap[i] + samples[i] * window_prev[i];
            }

            /* then from the spec - our window is
                  1.0 for samples 1024 <= n < 1472 (1024 - 128 / 2)
                  kbd/sin_right window for 1472 <= n < 1600
                  0 for 1600 <= n < 2048 */

            for(i=0;i<mid_len;i++) {
                overlap[i] = samples[long_len+i];
            }
            for(i=0;i<short_len;i++) {
                overlap[mid_len+i] = samples[long_len+mid_len+i] * window[short_len-i-1];
            }
            for(i=0;i<mid_len;i++) {
                overlap[mid_len+short_len+i] = MAAC_FLT_C(0.0);
            }

            break;
        }

        case MAAC_WINDOW_SEQUENCE_EIGHT_SHORT: {
            /* TODO should I just have cofficients decoder just write to
             window offsets of 256 instead of 128? Unsure if that would make
             things like mid/side processing more complex */
            maac_memcpy(&samples[7 * 2 * short_len], &samples[7 * short_len], sizeof(maac_flt) * short_len);
            maac_memcpy(&samples[6 * 2 * short_len], &samples[6 * short_len], sizeof(maac_flt) * short_len);
            maac_memcpy(&samples[5 * 2 * short_len], &samples[5 * short_len], sizeof(maac_flt) * short_len);
            maac_memcpy(&samples[4 * 2 * short_len], &samples[4 * short_len], sizeof(maac_flt) * short_len);
            maac_memcpy(&samples[3 * 2 * short_len], &samples[3 * short_len], sizeof(maac_flt) * short_len);
            maac_memcpy(&samples[2 * 2 * short_len], &samples[2 * short_len], sizeof(maac_flt) * short_len);
            maac_memcpy(&samples[1 * 2 * short_len], &samples[1 * short_len], sizeof(maac_flt) * short_len);
            /* 0 just stays in-place */

            /* now for the 8 small IMDCTs */
            maac_imdct(&samples[0 * 2 * short_len], 2 * short_len);
            maac_imdct(&samples[1 * 2 * short_len], 2 * short_len);
            maac_imdct(&samples[2 * 2 * short_len], 2 * short_len);
            maac_imdct(&samples[3 * 2 * short_len], 2 * short_len);
            maac_imdct(&samples[4 * 2 * short_len], 2 * short_len);
            maac_imdct(&samples[5 * 2 * short_len], 2 * short_len);
            maac_imdct(&samples[6 * 2 * short_len], 2 * short_len);
            maac_imdct(&samples[7 * 2 * short_len], 2 * short_len);

            window = window_shape == 1 ? maac_window_kbd_128 : maac_window_sin_128;
            window_prev = window_shape_prev == 1 ? maac_window_kbd_128 : maac_window_sin_128;

            /* these are the overlap and add values - which means what to add to overlap
            * for    0 <= n <  448, 0
            * for  448 <= n <  576, samples[n-488]  * W0[ n-448] *** w0 is previous window here and only here
            * for  576 <= n <  704, samples[n-488]  * W0[ n-448] + samples[ n-576] * W1[ n-576]
            * for  704 <= n <  832, samples[n-576]  * W1[ n-576] + samples[ n-704] * W2[ n-704]
            * for  832 <= n <  960, samples[n-704]  * W2[ n-704] + samples[ n-832] * W3[ n-832]
            * for  960 <= n < 1088, samples[n-832]  * W3[ n-832] + samples[ n-960] * W4[ n-960]
            * for 1088 <= n < 1216, samples[n-960]  * W4[ n-960] + samples[n-1088] * W5[n-1088]
            * for 1216 <= n < 1344, samples[n-1088] * W5[n-1088] + samples[n-1216] * W6[n-1216]
            * for 1344 <= n < 1472, samples[n-1216] * W6[n-1216] + samples[n-1344] * W7[n-1344]
            * for 1472 <= n < 1600, samples[n-1344] * W7[n-1344]
            * for 1600 <= n < 2047, 0
            * What's confusing is MPEG notates all of this as like:
                samples[i,n-448] (for first sample)
                samples[i,n-576] (for second sample)
                but - they *really* mean
                samples[ (window * 128) + (n-448) ]
                samples[ (window * 128) + (n-576) ]
                so you end up reading out of
                samples[(1 * 128) + 0] and
                samples[(2 * 128) + 0]
                I think they wrote all this assuming you're really tracking 8 different windows
                with individual indexes which, no.

                So anyways we can express all of these as (1 * 128), (2 * 128), etc

              To spell it out more explicitly in terms of what samples are written/read from
              for each of those chunks:

              - write to    0 <  448, read from     overlap
              - write to  448 <  576, read from     0 < 128
              - write to  576 <  704, read from   128 < 384
              - write to  704 <  832, read from   384 < 640
              - write to  832 <  960, read from   640 < 896
              - write to  960 < 1088, read from   896 < 960
              - write to 1088 < 1216, read from 1152 < 1408
              - write to 1216 < 1344, read from 1408 < 1664
              - write to 1344 < 1472, read from 1664 < 1920
              - write to 1472 < 1600, read from 1920 < 2048
              - write to 1600 < 2047, read from nowhere (just write 0s)

              In order to do this in-place with a single buffer, we need to avoid writing to somewhere
              we want to read from later.

            */

            for(i=0;i<short_len;i++) {
                /* write to 0 < 128, read from 0 < 128 */
                samples[i+(0*short_len)] =
                    overlap[mid_len+i+(0*short_len)] +
                    samples[i+(0*short_len)] * window_prev[i]
                ;

                /* write to 128 < 256, read from 128 < 384
                   final destination is 576 < 704 */
                samples[i+(1*short_len)] =
                    overlap[mid_len+i+(1*short_len)] +
                    samples[i+(1*short_len)] * window[short_len-1-i] +
                    samples[i+(2*short_len)] * window[i]
                ;

                /* write to 384 < 512, read from 384 < 640
                   final destination is samples[704 -> 832] */
                samples[i+(3*short_len)] =
                    overlap[mid_len+i+(2*short_len)] +
                    samples[i+(3*short_len)] * window[short_len-1-i] + 
                    samples[i+(4*short_len)] * window[i]
                ;

                /* write to 640 < 768, read from 640 < 896  
                   final destination is samples[832 -> 960] */
                samples[i+(5*short_len)] =
                    overlap[mid_len+i+(3*short_len)] +
                    samples[i+(5*short_len)] * window[short_len-1-i] +
                    samples[i+(6*short_len)] * window[i]
                ;

                /* write to 896 < 1088, read from 896 < 1152 */
                /* final destination is split:
                  samples[960 -> 1024]
                  overlap[0-64] */
                samples[i+(7*short_len)] =
                    (i < trans_len ? overlap[mid_len+i+(4*short_len)] : MAAC_FLT_C(0.0)) +
                    samples[i+(7*short_len)] * window[short_len-1-i] +
                    samples[i+(8*short_len)] * window[i]
                ;

                /* write to 1152 - 1280, read from 1152 - 1408 */
                /* final destination is overlap[64-192] */
                samples[i+(9*short_len)] =
                    samples[i+(9*short_len)] * window[short_len-1-i] +
                    samples[i+(10*short_len)] * window[i]
                ;

                /* write to 1408 - 1536, read from 1408 - 1664 */
                /* final destination is overlap[192-320] */
                samples[i+(11*short_len)] =
                    samples[i+(11*short_len)] * window[short_len-1-i] +
                    samples[i+(12*short_len)] * window[i]
                ;

                /* write to 1664 < 1792, read from 1664 < 1920,
                   final destination is overlap [320-448] */
                samples[i+(13*short_len)] =
                    samples[i+(13*short_len)] * window[short_len-1-i] +
                    samples[i+(14*short_len)] * window[i]
                ;

                /* write to 1920 < 2048, read from 1920 < 2048,
                   final destination is overlap[448-576] */
                samples[i+(15*short_len)] =
                    samples[i+(15*short_len)] * window[short_len-1-i]
                ;
            }

            /* first we scooch all the higher parts up together to clear some room */
            maac_memcpy(&samples[(14 * short_len)], &samples[(13 * short_len)], sizeof(maac_flt) * short_len);
            maac_memcpy(&samples[(13 * short_len)], &samples[(11 * short_len)], sizeof(maac_flt) * short_len);
            maac_memcpy(&samples[(12 * short_len)], &samples[(9 * short_len)],  sizeof(maac_flt) * short_len);
            maac_memcpy(&samples[trans_len + (11 * short_len)],  &samples[trans_len + (7 * short_len)],  sizeof(maac_flt) * trans_len);

            /* now samples[1472 - 2048] is together, and will become overlap[0-576] later */
            /* so we can bump up and collapse buffer[0-960] to buffer[448-1024] */

            /* copies 896 - 960 to 960 - 1024 */
            maac_memcpy(&samples[mid_len + (4 * short_len)],  &samples[(7 * short_len)],  sizeof(maac_flt) * trans_len);

            /* copies 640 < 768 to 832 < 960 */
            maac_memcpy(&samples[mid_len + (3 * short_len)],  &samples[(5 * short_len)],  sizeof(maac_flt) * short_len);

            /* copies 384 < 512 to 704 < 832 */
            maac_memcpy(&samples[mid_len + (2 * short_len)],  &samples[(3 * short_len)],  sizeof(maac_flt) * short_len);

            /* copies 128 < 256 to 576 < 704 */
            maac_memcpy(&samples[mid_len + (1 * short_len)],  &samples[(1 * short_len)],  sizeof(maac_flt) * short_len);

            /* copies   0 < 128 to 448 < 576 */
            maac_memcpy(&samples[mid_len + (0 * short_len)],  &samples[(0 * short_len)],  sizeof(maac_flt) * short_len);

            /* write to 0 < 448, read from overlap */
            for(i=0;i<mid_len;i++) {
                samples[i] = overlap[i];
            }

            /* overlap[0-576] is located in [trans_len + mid_len + (4 * 128)] */
            maac_memcpy(&overlap[0], &samples[trans_len + (11 * short_len)], sizeof(maac_flt) * (mid_len + short_len));

            /* clear out the rest of overlap (576 - 1024) */
            for(i=0;i<mid_len;i++) {
                overlap[i+mid_len+short_len] = MAAC_FLT_C(0.0);
            }

            break;
        }

        case MAAC_WINDOW_SEQUENCE_LONG_STOP: {
            maac_imdct(samples, long_len*2);

            /* this is used to go from an eight-short back to an only-long */

            window = p->window_shape == 1 ? maac_window_kbd_1024 : maac_window_sin_1024;
            window_prev = p->window_shape_prev == 1 ? maac_window_kbd_128 : maac_window_sin_128;

            for(i=0;i<mid_len;i++) {
                samples[i] = overlap[i];
            }
            for(i=0;i<short_len;i++) {
                samples[i+mid_len] =
                    overlap[i+mid_len] +
                    samples[i+mid_len] * window_prev[i]
                ;
            }
            for(i=0;i<mid_len;i++) {
                samples[i+mid_len+short_len] =
                    overlap[i+mid_len+short_len] +
                    samples[i+mid_len+short_len]
                ;
            }
            for(i=0;i<long_len;i++) {
                overlap[i] = samples[long_len+i] * window[long_len-1-i];
            }
            break;
        }

    }

}
