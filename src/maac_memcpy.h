/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_MEMCPY_INCLUDE_GUARD
#define MAAC_MEMCPY_INCLUDE_GUARD

#include "maac_visibility.h"
#include "maac_restrict.h"
#include "maac_stddef.h"

MAAC_PRIVATE
void* maac_memcpy(void* maac_restrict _dest, const void* maac_restrict _src, size_t len);

#endif /* INCLUDE_GUARD */
