/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_CLAMP_INCLUDE_GUARD
#define MAAC_CLAMP_INCLUDE_GUARD

#define maac_clamp(val, minv, maxv) ( (val) > (maxv) ? (maxv) : (val) < (minv) ? (minv) : (val) )

#endif /* INCLUDE_GUARD */

