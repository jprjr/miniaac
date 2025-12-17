#include "maac_memcpy.h"

#ifdef MAAC_NO_STDSTRING

MAAC_PRIVATE
void* maac_memcpy(void* maac_restrict _dest, const void* maac_restrict _src, size_t len) {
    unsigned char* dest = (unsigned char *)_dest;
    const unsigned char* src = (const unsigned char *)_src;
    while(len--) {
        dest[len] = src[len];
    }
    return _dest;
}

#else

#include "maac_string.h"

MAAC_PRIVATE
void* maac_memcpy(void* maac_restrict _dest, const void* maac_restrict _src, size_t len) {
    return memcpy(_dest, _src, len);
}

#endif
