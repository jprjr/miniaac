#include <stdio.h>
#include <math.h>
#include <float.h>

int main(void) {
    int i;

    for(i=-3;i<4;i++) {
        printf("MAAC_F64_C(%#.*g)\n", DBL_DECIMAL_DIG, pow(2.0, ((double)i) / 4.0));
    }

    for(i=-3;i<4;i++) {
        printf("MAAC_F32_C(%#.*g)\n", FLT_DECIMAL_DIG, pow(2.0, ((double)i) / 4.0));
    }

    return 0;
}
