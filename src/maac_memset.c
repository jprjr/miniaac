/* SPDX-License-Identifier: 0BSD */
#include "maac_memset.h"

#ifdef MAAC_NO_STDSTRING

MAAC_PRIVATE
void* maac_memset(void* _dest, int val, size_t len)
{
    unsigned char* dest = (unsigned char *)_dest;
    unsigned char c = (unsigned char)val;
    while(len--) {
        dest[len] = c;
    }
    return _dest;
}

#else

#include "maac_string.h"

MAAC_PRIVATE
void* maac_memset(void* _dest, int val, size_t len) {
    return memset(_dest, val, len);
}

#endif
