/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_UNREACHABLE_INCLUDE_GUARD
#define MAAC_UNREACHABLE_INCLUDE_GUARD

#ifndef MAAC_UNREACHABLE_RETURN

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define MAAC_UNREACHABLE_RETURN(x) __builtin_unreachable()
#else
#define MAAC_UNREACHABLE_RETURN(x) return x
#endif

#endif /* MAAC_UNREACHABLE_RETURN */

#endif /* INCLUDE_GUARD */
