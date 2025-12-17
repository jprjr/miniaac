/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_BITFIELD_INCLUDE_GUARD
#define MAAC_BITFIELD_INCLUDE_GUARD

#include "maac_stdint.h"

#ifdef MAAC_COMPACT
#define MAAC_BITFIELD(t,x,bits) unsigned int x: bits
#else
#define MAAC_BITFIELD(t, x,bits) t x
#endif

#endif /* INCLUDE_GUARD */
