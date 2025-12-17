/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_PURE_INCLUDE_GUARD
#define MAAC_PURE_INCLUDE_GUARD

#ifndef maac_pure

#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5))
#define maac_pure __attribute__((__pure__))
#elif defined(__has_attribute)
#if __has_attribute(pure)
#define maac_pure __attribute__((pure))
#endif
#else
#define maac_pure
#endif

#endif /* maac_pure */

#endif /* INCLUDE_GUARD */

