/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_INLINE_INCLUDE_GUARD
#define MAAC_INLINE_INCLUDE_GUARD

#ifndef maac_inline

#ifdef __cplusplus
#define maac_inline inline
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define maac_inline inline
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
#define maac_inline __inline__
#elif defined(_MSC_VER) && _MSC_VER >= 1200
#define maac_inline __inline
#else
#define maac_inline
#endif

#endif /* maac_inline */

#endif /* INCLUDE_GUARD */
