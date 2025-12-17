/* SPDX-License-Identifier: 0BSD */
#include "maac_math.h"

#ifdef MAAC_NO_STDMATH
#include "maac_assert.h"
#include "maac_const.h"
#include "maac_memcpy.h"

#define maac_math_memcpy(x,y) maac_assert( sizeof(*(x)) == sizeof(*(y)) ); maac_memcpy( (x), (y), sizeof *(y))

#else

#ifdef __cplusplus
#include <cmath>
#else
#include <math.h>
#endif

#define maac_sqrt(x)   maac_flt_cast(sqrt(x))
#define maac_pow(x,n)  maac_flt_cast(pow(x,n))

#endif

MAAC_PRIVATE
maac_flt maac_inv_sqrt(maac_flt x) {
#ifdef MAAC_NO_STDMATH
    maac_u32 i;
    /* specifically using 32-bit floats here to avoid using int64 */
    maac_f32 y;
    maac_f32 fx;

    fx = (maac_f32)x;

    maac_math_memcpy(&i, &fx);
    i = 0x5F1FFFF9 - (i >> 1);
    maac_math_memcpy(&y, &i);

    fx = y * 0.703952253f * ( 2.38924456f - fx * y * y );
    return maac_flt_cast(fx);
#else
    return MAAC_FLT_C(1.0) / maac_sqrt(x);
#endif
}

#ifdef MAAC_NO_STDMATH
/* pre-comuted pow(2,-3.0/4.0) through pow(2,3.0/4.0) */
static const maac_flt maac_pow2_fourths[7] = {
#ifdef MAAC_DOUBLE_PRECISION
    MAAC_F64_C(0.59460355750136051),
    MAAC_F64_C(0.70710678118654757),
    MAAC_F64_C(0.84089641525371450),
    MAAC_F64_C(1.0000000000000000),
    MAAC_F64_C(1.1892071150027210),
    MAAC_F64_C(1.4142135623730951),
    MAAC_F64_C(1.6817928305074290)
#else
    MAAC_F32_C(0.594603558),
    MAAC_F32_C(0.707106781),
    MAAC_F32_C(0.840896415),
    MAAC_F32_C(1.00000000),
    MAAC_F32_C(1.18920712),
    MAAC_F32_C(1.41421356),
    MAAC_F32_C(1.68179283)
#endif
};
#endif

MAAC_PRIVATE
maac_flt maac_pow2_xdiv4(maac_s16 x) {
#ifdef MAAC_NO_STDMATH
    /* so scalefactors in AAC follow the pattern of being
      2.0^(x/4)

      There's also a 0.5^(x/4) used but we'll get to that in a second.

      2.0^(x/4) can be broken down, say x is 9 -- 9/4 is the same as 8/4 + 1/4.
      And x^(y+z) == x^y * x^z.

      Again with x=9 we can break this down into:
      2.0^(9/4)
          == 2.0^((8/4) + (1/4))
          == 2.0^(8/4) * 2.0^(1/4)
          == 2^2 * 2^(1/4)
      Oh would you look at that, it's an integer power of two and a non-integer
      power of 2, and integer powers of 2 are easy to compute. It's just 1 << exponent.

      This means we can think of this formula as:
      (1 << int(x/4)) * (2.0^(x % 4))

      So one option would be something like:

      float f = (float)((1 << x/4)) * (2.0^(x % 4))

      We can have pre-computed 2^(x/4) values, we only need 4.

      Now for a negative power - say x was  -9, so our exponent is -2,
      you just compute the positive power then inverse, so:

      2^-2 == 1.0 / (2^2)

      So we *could* do something like:
      float f = ((float)(1 << abs(x)/4)) * (2.0^(abs(x) % 4))
      if(x < 0) f = 1.0/f;

      But what's great about power-of-two floats is they're all zero bits
      except the exponent section, so instead of branching on whether
      or not x is positive/negative, we can just take the base
      exponent value of 127 and add (x/4).

      If x is negative, we get a lower exponent and the equivalent of
      1.0 / (1 << exp). If x is positive, we get 1 << exp.

      So now we've got our power-of-two float, we just need to multiply
      with a pre-computed pow(2,(x%4)/4 and blammo, we have our value
      without resorting to a huge lookup table.

      Which is *great* because one thing about the AAC standards that drive
      me nuts is - I can't find limits. If somebody could tell me where
      it says "scale factors are in the range x to y" that would be great,
      because then I could make lookup tables with confidence.

      For handling 0.5^(x/4) we instead compute 2^(-x/4), since that
      computes 1/(2^(x/4)) - which is equivalent.
    */

    const maac_s32 x4 = x / 4;
    maac_s32 exp = 127;
    maac_u32 u;
    maac_f32 f;

    /*  ensure we don't overflow/underflow the exponent part of the float -
        exponent is 8 bits with 0 and 255 being reserved and 127 = 1.0,
        ensure we remain under 255 and over 0 */
    maac_assert(x4 < 128);
    maac_assert(x4 > -127);

    u = ((exp + x4) << 23) & MAAC_U32_C(0x7F800000);
    maac_math_memcpy(&f, &u);

    return maac_flt_cast(f) * maac_pow2_fourths[(x % 4)+3];
#else
    return maac_pow(MAAC_FLT_C(2.0), MAAC_FLT_C(0.25) * (maac_flt_cast(x)));
#endif
}

#ifdef MAAC_NO_STDMATH
static maac_f32
maac_frexpf(maac_f32 x, int* e) {
    /* extract the 8 exponent bits */
    maac_u32 u;
    maac_u32 t;

    maac_math_memcpy(&u, &x);
    t = u >> 23 & 0xff;

    /* if our exponent is zero, we're either dealing with 0 or a subnormal */
    if(t == 0) {
        if(x == 0.0f) { /* phew! */
            *e = 0;
            return x;
        }

        /* scale up by 2^64 (exponent = 191) */
        x = maac_frexpf(x * MAAC_F32_C(18446744073709551616.0), e);
        *e -= 64;
        return x;
    } else if(t == 0xff) {
        /* infinity or nan */
        return x;
    }

    /* we want to return a fraction and exponent such that
    frac * 2^exp = arg,
    with frac being in the range of 0.5,1.0 - with the 1.0 values excluded.

    So what we're *really* returning is something like:
    frac = ??
    exp = ceil(log2(x))

    the output range will be 1 to 128 */
    
    *e = t - 126; /* this is equivalent of ceil(log2(x)) - basically converting x
    into base2 without having to get into implementing log2 */

    /* clear out the original exponent bits, keep sign and mantissa*/
    u &= MAAC_U32_C(0x807fffff);
    /* set the exponent to 126 - so now we have a value in the range of 0.5 -> 1.0 */
    u |= MAAC_U32_C(0x3f000000);
    maac_math_memcpy(&x, &u);
    return x;
}

maac_const
static
maac_f32
maac_ldexpf(maac_f32 x, int n) {
    maac_f32 u;
    maac_f32 t1;
    maac_f32 t2;
    maac_f32 y;
    maac_u32 rem;

    y = x;

    /* similar to  above - we shouldn't ever have particularly large values of n
       since our input ranges will only be 0 - 8191 */
    if(n > 127) {
        rem = MAAC_U32_C(0x7f000000); /* equivalent to 0x1p127f */
        maac_math_memcpy(&t1, &rem);

        rem = 2;
        while(rem--) {
            y *= t1;
            n -= 127;
            if(n <= 127) break;
        }
        if(n > 127) n = 127;
    } else if(n < -126) {
        rem = MAAC_U32_C(0x00800000); /* equivalent to 0x1p-126f */
        maac_math_memcpy(&t1, &rem);
        rem = MAAC_U32_C(0x4b800000); /* equivalent to 0x1p24f */
        maac_math_memcpy(&t2, &rem);
        rem = 2;
        while(rem--) {
            y *= t1 - t2;
            n += 126 - 24;
            if(n >= -126) break;
        }
        if(n < -126) n = -126;
    }
    rem = (MAAC_U32_C(0x7f) + n) << 23;
    maac_math_memcpy(&u, &rem);
    return y * u;
}
#endif

MAAC_PRIVATE
maac_flt maac_cbrt(maac_u16 x) {
#ifdef MAAC_NO_STDMATH
/* https://people.freebsd.org/~lstewart/references/apple_tr_kt32_cuberoot.pdf */
    maac_s32 exp;
    maac_s32 shx;
    maac_f32 fx;
    maac_f32 fr;
    maac_f32 r;

    if(x == 0) return MAAC_FLT_C(0.0);

    fx = maac_f32_cast(x);

    fr = maac_frexpf(fx, &exp);
    shx = exp % 3;
    if(shx) {
        shx -= 3;
    }
    exp = (exp - shx) / 3;
    fr = maac_ldexpf(fr, shx);

    fr = (-0.46946116f * fr + 1.072302f) * fr + 0.3812513f;
    r = maac_ldexpf(fr, exp);

    r = (2.0f/3.0f) * r + (1.0f/3.0f) * fx / (r * r);
    r = (2.0f/3.0f) * r + (1.0f/3.0f) * fx / (r * r);

    return maac_flt_cast(r);
#else
    return maac_pow(maac_flt_cast(x), MAAC_FLT_C(1.0) / MAAC_FLT_C(3.0));
#endif
}

