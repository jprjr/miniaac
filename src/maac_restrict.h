/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_RESTRICT_INCLUDE_GUARD
#define MAAC_RESTRICT_INCLUDE_GUARD

#ifndef maac_restrict

#if defined(__STDC_VERSION__) && __STDC__VERSION__ >= 199901L
#define maac_restrict restrict
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define maac_restrict __restrict
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
#define maac_restrict __restrict
#else
#define maac_restrict
#endif

#endif /* maac_restrict */

#endif /* INCLUDE_GUARD */
