/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_ALIGN_INCLUDE_GUARD
#define MAAC_ALIGN_INCLUDE_GUARD

#include "maac_stddef.h"
#include "maac_pure.h"
#include "maac_inline.h"

static maac_inline
void* maac_align(void* ptr, size_t align) {
    unsigned char* mem;
    size_t s;
    size_t o;

    if(align <= 1) return ptr;

    mem = (unsigned char *)ptr;
    s = (size_t)mem;

    o = ((s + align - 1) & (~(align-1))) - s;

    return (void*)&mem[o];
}

#endif /* INCLUDE_GUARD */
