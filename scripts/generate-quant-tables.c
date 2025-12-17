#include <stdio.h>
#include <math.h>
#include <float.h>

int main(void) {
    const unsigned int start = 0;
    const unsigned int end = 8192;

    unsigned int i;

    printf("/* SPDX-License-Identifier: 0BSD */\n");
    printf("#ifndef MAAC_INV_QUANT_TABLES_INCLUDE_GUARD\n");
    printf("#define MAAC_INV_QUANT_TABLES_INCLUDE_GUARD\n\n");
    printf("#include \"maac_float.h\"\n\n");

    printf("static const maac_flt MAAC_INV_QUANT[%u] = {", end - start);
    for(i = start; i < end; i++) {
        double id = (double)i;
        id = pow(id, 4.0/3.0);
        if(i > 0) {
            printf(",");
        }
        if(i % 4 == 0) {
            printf("\n    ");
        }
        printf("MAAC_FLT_C(%#.*g)", DBL_DECIMAL_DIG, id);
    }

    printf("\n};\n\n");
    printf("#endif /* INCLUDE_GUARD */\n");

    return 0;
}

