/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_STDDEF_INCLUDE_GUARD
#define MAAC_STDDEF_INCLUDE_GUARD

/* MAAC_EXCLUDE_BEGIN
*
*  This header is kinda silly - but this way instead of requiring
*  different processing of #include<> directives in my build script,
*  I can just include this guy and ensure stddef.h is only included
*  once.
*
MAAC_EXCLUDE_END */

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#endif /* INCLUDE_GUARD */
