/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_RESULT_EXTRAS_INCLUDE_GUARD
#define MAAC_RESULT_EXTRAS_INCLUDE_GUARD

#include "maac_stddef.h"
#include "maac_stdint.h"
#include "maac_visibility.h"
#include "maac_cdecls.h"
#include "maac_const.h"

MAAC_CDECLS_BEGIN

maac_const
MAAC_PUBLIC
const char*
maac_result_name(const maac_s32 result);

maac_const
MAAC_PUBLIC
size_t
maac_result_name_len(const maac_s32 result);

MAAC_CDECLS_END

#endif /* INCLUDE_GUARD */
