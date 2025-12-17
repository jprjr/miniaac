/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_ASSERT_INCLUDE_GUARD
#define MAAC_ASSERT_INCLUDE_GUARD

#ifdef MAAC_ENABLE_ASSERT

#ifdef __cplusplus
#include <cassert>
#else
#include <assert.h>
#endif

#define maac_assert(x) assert( (x) )

#else

#define maac_assert(x)

#endif

#endif /* INCLUDE_GUARD */

