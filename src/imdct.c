#include "imdct.h"

#include "imdct_twiddle_factors.h"
#include "bit_reverse_data.h"
#include "maac_assert.h"

/* take 1, taking the reference formula, hoisting a few
const values up. Pretty dang slow. */

#if 0
#include <math.h> /* for cos() */
#include "maac_memcpy.h"
#include "maac_math.h"

    /* for reference formula is:
      n0 = (len / 2 + 1) / 2

      x[i] = 2/len *
        sum{k: 0 -> len/2} of:
          in[k] *
          cos(
            (2 * pi / len)
            *
            (i + n0)
            *
            (k + 0.5)
          )
    */

static maac_flt maac_imdct_value(const maac_flt* buf, const maac_u16 len, maac_u16 idx, const maac_flt n0, const maac_flt pi2n) {
   maac_u16 k  = 0;
   maac_flt sum = MAAC_FLT_C(0.0);

   for(k=0;k<len/2;k++) {
       sum += buf[k] * cos(pi2n * (maac_flt_cast(idx) + n0) * (maac_flt_cast(k) + MAAC_FLT_C(0.5)));
   }

   return sum;
}

MAAC_PRIVATE
void
maac_imdct(maac_flt* buf,  maac_u16 len) {
   const maac_flt n0 = (((maac_flt_cast(len)) / MAAC_FLT_C(2.0)) + MAAC_FLT_C(1.0)) / MAAC_FLT_C(2.0);
   const maac_flt pi2n = 2.0f * MAAC_M_PI / maac_flt_cast(len);
   const maac_flt scale = 2.0f / maac_flt_cast(len);
   maac_flt copy[1024];
   maac_u16 i = 0;

   maac_memcpy(copy, buf, sizeof(maac_flt) * ((maac_u32)len/2) );

   i = 0;
   while(i < len) {
       buf[i] = maac_imdct_value(copy, len, i, n0, pi2n) * scale;
       i++;
   }
}

#else

/* take 2 - let's try adapting
"The use of multirate filter banks for coding of high quality digital audio"
by Sporer, Brandenburg, and Edler (1992) */
static 
maac_u16
maac_ilog2(maac_u16 v) {
    maac_u16 r = 0;
    while(v >>= 1) {
        r++;
    }
    return r;
}

static void
maac_imdct_step0(maac_flt* buf, const maac_u16 n) {
    const maac_u16 n2 = n >> 1;
    /* TODO - should we scale input down? */
    const maac_flt scale = MAAC_FLT_C(1.0);

    maac_u16 k = 0;
    while(k < n2) {
        buf[k] *= scale;
        k++;
    }
    while(k < n) {
        buf[k] = -buf[n - k - 1];
        k++;
    }
}

static void
maac_imdct_step1(maac_flt* buf, const maac_flt* A, const maac_u16 n) {
    const maac_u16 n4 = n >> 2;
    maac_u16 k = 0;
    maac_u16 k2 = 0;
    maac_u16 k4 = 0;

    maac_flt a, b, c, d;

    /* I think the paper has a rendering error - it shows
    the formula as:
      buf[x]  (buf[q] - buf[t] * a) -
              -(buf[r] - buf[s]) * a
              but I think it's just supposed to be a regular minus -
              not subtracting a negation. */

    while(k < n4) {
        /* we're working backwards from end of buffer in increments of 4 */
        /* using some numbers of n=2048, first iteration we're performing:
          buf[2047] = (buf[0] - buf[2047] * A
                      -
                      (buf[2] - buf[2045]) * A

          buf[2045] = (buf[0] - buf[2047] * A
                      +
                      (buf[2] - buf[2045]) * A 

           last iteration we're performing:
          buf[[3]   = (buf[2044] - buf[3]) * A
                      -
                      (buf[2046] - buf[1]) * A

          buf[1]  = buf[2044] - buf[3] * A
                    +
                    buf[2046] - buf[1] * A

        We're only writing to the odd indexes so we don't
        have to worry about preserving even indexes, we just
        need to preserve the two odd indexes we read from
        since we overwrite mid-step. But we'll copy out all
        four values into variables since that's easier to read. */

        a = buf[n - k4 - 1];
        b = buf[n - k4 - 3];
        c = buf[k4];
        d = buf[k4+2];

        buf[n - k4 - 1] =
            (c - a) * A[k2]
            -
            (d - b) * A[k2+1];

        buf[n - k4 - 3] =
            (c - a) * A[k2 + 1]
            +
            (d - b) * A[k2];

        k++;
        k2 += 2;
        k4 += 4;
    }
}

static void
maac_imdct_step2(maac_flt* buf, const maac_flt* A, const maac_u16 n) {
    const maac_u16 n2 = n >> 1;
    const maac_u16 n8 = n >> 3;
    maac_u16 k = 0;
    maac_u16 k2 = 0;
    maac_u16 k4 = 0;

    maac_flt a, b, c, d;

    while(k < n8) {
        /* this time I'm just going to write out the buffer indexes accessd.

        first iteration
          buf[1027] = stuff(buf[1027],buf[3])
          buf[1025] = stuff(buf[1025],buf[1])
          buf[3] = stuff(buf[1027],buf[3],buf[1025],buf[1])
          buf[1] = stuff(buf[1025],buf[1],buf[1027],buf[3])

        last iteration:
          buf[2047] = stuff(buf[2047],buf[1023])
          buf[2045] = stuff(buf[2045],buf[1021])
          buf[1023] = stuff(buf[2047],buf[1023],buf[2045],buf[1])
          buf[1021] = stuff(buf[2045],buf[1021],buf[2047],buf[3])
        so similar to last time - we only write to odd indexes,
        and we do overwrite stuff mid-update so we'll save to variables */
        a = buf[n2 + k4 + 3];
        b = buf[n2 + k4 + 1];
        c = buf[k4 + 3];
        d = buf[k4 + 1];

        buf[n2 + k4 + 3] =
          a + c;
        buf[n2 + k4 + 1] =
          b + d;
        buf[k4 + 3] =
          (a - c) * A[n2 - 4 - k4]
          -
          (b - d) * A[n2 - 3 - k4];
        buf[k4 + 1] =
          (b - d) * A[n2 - 4 - k4]
          +
          (a - c) * A[n2 - 3 - k4];

        k++;
        k2 += 2;
        k4 += 4;
    }
}

static void
maac_imdct_step3(maac_flt* buf, const maac_flt* A, const maac_u16 n) {
    const maac_u16 ld = maac_ilog2(n);

    maac_u16 l = 0;
    maac_u16 r = 0;
    maac_u16 r4 = 0;
    maac_u16 s = 0;
    maac_u16 s2 = 0;

    maac_flt a, b, c, d;

    while(l < (ld - 3)) {
        const maac_u16 k0 = n >> (l + 2); /* equivalent to n / (2^(l+2)) */
        const maac_u16 k1 = 1 << (l + 3);
        const maac_u16 r_end = n >> (l + 4); /* equivalent to n / (2^(l+4)) */
        const maac_u16 s_end = 1 << (l + 1);

        while(r < r_end) {
            while(s < s_end) {
                /* like previous entries - this will only access odd indexes */
                a = buf[n - 1 - k0 * s2 - r4];
                b = buf[n - 3 - k0 * s2 - r4];
                c = buf[n - 1 - k0 * (s2 + 1) - r4];
                d = buf[n - 3 - k0 * (s2 + 1) - r4];

                buf[n - 1 - k0 * s2 - r4] =
                  a + c;

                buf[n - 3 - k0 * s2 - r4] =
                  b + d;

                buf[n - 1 - k0 * (s2 + 1) - r4] =
                  (a - c) * A[r * k1]
                  -
                  (b - d) * A[r * k1 + 1];

                buf[n - 3 - k0 * (s2 + 1) - r4] =
                  (b - d) * A[r * k1]
                  +
                  (a - c) * A[r * k1 + 1];
                s++;
                s2 += 2;
            }
            s=0;
            s2=0;
            r++;
            r4 += 4;
        }
        r=0;
        r4=0;
        l++;
    }
}

static maac_u16
maac_bit_reverse(maac_u16 n) {
    return
      (((maac_u16)maac_bit_reverse_data[n & 0xff]) << 8)
      |
      ((maac_u16)maac_bit_reverse_data[n >> 8]);
}

static void
maac_imdct_step4(maac_flt* buf, const maac_u16 n) {
    const maac_u16 n8 = n >> 3;
    /* needed to properly scale bits back into range */
    const maac_u16 ld = maac_ilog2(n);
    maac_u16 i;
    maac_u16 i8;
    maac_u16 j;
    maac_u16 j8;

    maac_flt f[8];

    /* we skip 0 and 255 */
    i=1;
    while(i < n8 - 1) {
        j = maac_bit_reverse(i) >> (16 - ld + 3);
        if(i < j) {
            i8 = 8 * i;
            j8 = 8 * j;

            f[0] = buf[i8+1];
            f[1] = buf[i8+3];
            f[2] = buf[i8+5];
            f[3] = buf[i8+7];
            f[4] = buf[j8+1];
            f[5] = buf[j8+3];
            f[6] = buf[j8+5];
            f[7] = buf[j8+7];

            buf[j8+1]=f[0];
            buf[j8+3]=f[1];
            buf[j8+5]=f[2];
            buf[j8+7]=f[3];
            buf[i8+1]=f[4];
            buf[i8+3]=f[5];
            buf[i8+5]=f[6];
            buf[i8+7]=f[7];
        }

        i++;
    }
}

static void
maac_imdct_step5(maac_flt* buf, const maac_u16 n) {
    const maac_u16 n2 = n >> 1;
    maac_u16 k = 0;
    maac_u16 k2 = 0;

    while(k < n2) {
        buf[k] = buf[k2+1];

        k++;
        k2 += 2;
    }
}

static void
maac_imdct_step6(maac_flt* buf, const maac_u16 n) {
    const maac_u16 n8 = n >> 3;
    const maac_u16 n3_4 = n - (n >> 2);

    maac_u16 k = 0;
    maac_u16 k2 = 0;
    maac_u16 k4 = 0;

    while(k < n8) {
        buf[n - 1 - k2] = buf[k4];
        buf[n - 2 - k2] = buf[k4+1];
        buf[n3_4 - 1 - k2] = buf[k4 + 2];
        buf[n3_4 - 2 - k2] = buf[k4 + 3];

        k++;
        k2 += 2;
        k4 += 4;
    }
}

static void
maac_imdct_step7(maac_flt* buf, const maac_flt* C, const maac_u16 n) {
    const maac_u16 n2 = n >> 1;
    const maac_u16 n8 = n >> 3;

    maac_u16 k = 0;
    maac_u16 k2 = 0;

    maac_flt a, b, c, d;

    while(k < n8) {
        a = buf[n2 + k2];
        b = buf[n2 + k2 + 1];
        c = buf[n - 2 - k2];
        d = buf[n - 1 - k2];

        buf[n2 + k2] =
          (a + c + C[k2+1] * (a - c) + C[k2] * (b + d)) / MAAC_FLT_C(2.0);

        buf[n - 2 - k2] =
          (a + c - C[k2+1] * (a - c) - C[k2] * (b + d)) / MAAC_FLT_C(2.0);

        buf[n2 + k2 + 1] =
          (b - d + C[k2+1] * (b + d) - C[k2] * (a - c)) / MAAC_FLT_C(2.0);

        buf[n - 1 - k2] =
          (-b + d + C[k2+1] * (b + d) - C[k2] * (a - c)) / MAAC_FLT_C(2.0);

        k++;
        k2 += 2;
    }
}

static void
maac_imdct_step8(maac_flt* buf, const maac_flt* B, const maac_u16 n) {
    const maac_u16 n2 = n >> 1;
    const maac_u16 n4 = n >> 2;

    maac_u16 k = 0;
    maac_u16 k2 = 0;

    maac_flt a, b;

    while(k < n4) {
        a = buf[k2 + n2];
        b = buf[k2 + 1 + n2];

        buf[k] = a * B[k2] + b * B[k2 + 1];
        buf[n2 - 1 - k] = a * B[k2 + 1] - b * B[k2];

        k++;
        k2 += 2;
    }
}

static void
maac_imdct_finalize(maac_flt* buf, maac_flt scale, const maac_u16 n) {
    const maac_u16 n2 = n >> 1;
    const maac_u16 n4 = n >> 2;
    const maac_u16 n3_4 = n - n4;

    /* original formula goes:
       while(k < n4) {
           buf[0] = buf[512]
           ..
           buf[511] = buf[1023]
       }

       while(k < n3_4) {
           buf[512]  = -buf[1023]
           buf[513]  = -buf[1022]
           buf[1023] = -buf[512]
           buf[1024] = -buf[511
           ..
           buf[1535] = -buf[0]
       }
       while(k < n) {
           buf[1536] = -buf[0]
           ..
           buf[2047] = -buf[511]
           k++;
       }
       which would have us overwrite values as we use them so instead:
           - update 1536 -> 2047 and 0 -> 511
           - update 512 -> 1023 and 1024 -> 1535
   */

   maac_u16 k = 0;
   while(k < n4) {
       /* copy -buf[0] -> buf[1546] */
       buf[n3_4 + k] = -buf[k] * scale;
       /* copy buf[512] -> buf[0] */
       buf[k] = buf[k + n4] * scale;
       k++;
   }
   /* so now:
     1546 -> 2047 contain what was in   0 -> 511 (negative)
     0    ->  511 contain what was in 512 -> 1023 (original)
     and everything we needed was in 0 -> 1023 originally, so
     we just have to mess with what indexes we read from
   */

   while(k < n2) {
       /* 512 needs to be -1023, which is now in 511 */
       buf[k] = -buf[n2 -k - 1];

       /* 1024 needs to be -511 - which is in 2047 and already negated */
       buf[k + n4] = buf[n + n4 - k - 1];

       k++;
   }

}

MAAC_PRIVATE
void
maac_imdct(maac_flt* samples, const maac_u16 n) {
    const maac_flt* A = n == 2048 ? MAAC_IMDCT_A_2048 : MAAC_IMDCT_A_256;
    const maac_flt* B = n == 2048 ? MAAC_IMDCT_B_2048 : MAAC_IMDCT_B_256;
    const maac_flt* C = n == 2048 ? MAAC_IMDCT_C_2048 : MAAC_IMDCT_C_256;

    maac_imdct_step0(samples, n);
    maac_imdct_step1(samples, A, n);
    maac_imdct_step2(samples, A, n);
    maac_imdct_step3(samples, A, n);
    maac_imdct_step4(samples, n);
    maac_imdct_step5(samples, n);
    maac_imdct_step6(samples, n);
    maac_imdct_step7(samples, C, n);
    maac_imdct_step8(samples, B, n);

    /* TODO the scalefactor in the paper's IMDCT is n/4,
    but MPEG's is 2/n - I would think that 2/n * n/4 would come
    out to 1/4 but that's definitely not it.
    Experimentation shows that 1/n works, but... why? */
    maac_imdct_finalize(samples, MAAC_FLT_C(1.0) / maac_flt_cast(n), n);
}
#endif
