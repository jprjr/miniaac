#ifdef __cplusplus
#include <cstdio>
#else
#include <stdio.h>
#endif

int main(void) {
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int b = 0;

    printf("/* SPDX-License-Identifier: 0BSD */\n");
    printf("#ifndef MAAC_BIT_REVERSE_DATA_INCLUDE_GUARD\n");
    printf("#define MAAC_BIT_REVERSE_DATA_INCLUDE_GUARD\n\n");
    printf("#include \"maac_stdint.h\"\n\n");

    printf("static const maac_u8 maac_bit_reverse_data[256] = {");

    for(i=0;i<256;i++) {
        j = 0;
        j |= (i & 0x01) << 7;
        j |= (i & 0x02) << 5;
        j |= (i & 0x04) << 3;
        j |= (i & 0x08) << 1;
        j |= (i & 0x10) >> 1;
        j |= (i & 0x20) >> 3;
        j |= (i & 0x40) >> 5;
        j |= (i & 0x80) >> 7;
        if(i > 0) {
            printf(",");
        }
        printf("\n    /* ");
        b = 8;
        while(b--) {
            printf("%u", (i >> b) & 0x01);
        }
        printf(" => ");
        b = 8;
        while(b--) {
            printf("%u", (j >> b) & 0x01);
        }
        printf(" */ 0x%02x", j);
    }

    printf("\n};\n\n");
    printf("\n#endif /* INCLUDE_GUARD */\n");

    return 0;
}
