/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_VISIBILITY_INCLUDE_GUARD
#define MAAC_VISIBILITY_INCLUDE_GUARD

#ifndef MAAC_PUBLIC

#if defined(__GNUC__) && __GNUC__ > 4
#define MAAC_PUBLIC __attribute__ ((visibility("default")))
#else
#define MAAC_PUBLIC
#endif

#endif /* MAAC_PUBLIC */

#ifndef MAAC_PRIVATE

#if defined(__GNUC__) && __GNUC__ > 4
#define MAAC_PRIVATE __attribute__ ((visibility("hidden")))
#else
#define MAAC_PRIVATE
#endif

#endif /* MAAC_PRIVATE */

#endif /* INCLUDE_GUARD */
