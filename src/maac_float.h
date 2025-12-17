/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_FLOAT_INCLUDE_GUARD
#define MAAC_FLOAT_INCLUDE_GUARD

/* TODO - maybe add integer-based math options? */
#ifdef __cplusplus
#include <cfloat>
#else
#include <float.h>
#endif

typedef float  maac_f32;
typedef double maac_f64;

#define MAAC_F32_C(x) (x ## f)
#define MAAC_F64_C(x) (x)

#define maac_f32_cast(x) ((maac_f32)x)
#define maac_f64_cast(x) ((maac_f64)x)

#ifdef MAAC_DOUBLE_PRECISION
typedef maac_f64 maac_flt;
#define MAAC_FLT_C(x) MAAC_F64_C(x)
#else
typedef maac_f32 maac_flt;
#define MAAC_FLT_C(x) MAAC_F32_C(x)
#endif

#define maac_flt_cast(x) ((maac_flt)x)

#endif /* INCLUDE_GUARD */
