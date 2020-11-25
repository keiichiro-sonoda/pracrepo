#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(SEED);
    srand((unsigned)time(NULL));
    initBoard();
    char format[] = FNF_L1RR05006000000005;
    //makeFGFilePrm1L(format);
    //checkPrm1LFile(format, 1);
    // 500 世代に挑戦
    //nGenePrm1LLoopSeed(rltUniRd, format, 0, 0, 500);
    // for debugging
    printString(format);
    u_char uc1, uc2;
    float f1, f2;
    for (uc1 = 0; uc1 < 255; uc1++) {
        f1 = uchar2weight(uc1);
        uc2 = weight2uchar(f1);
        f2 = uchar2weight(uc2);
        printf("%d, %f, %d, %f\n", (int)uc1, f1, (int)uc2, f2);
        if (uc1 != uc2)
            printf("NG!\n");
    }
    return 0;
}