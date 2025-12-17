/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_CONST_INCLUDE_GUARD
#define MAAC_CONST_INCLUDE_GUARD

#ifndef maac_const

#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5))
#define maac_const __attribute__((__const__))
#elif defined(__has_attribute)
#if __has_attribute(const)
#define maac_const __attribute__((const))
#endif
#else
#define maac_const
#endif

#endif /* maac_const */

#endif /* INCLUDE_GUARD */
