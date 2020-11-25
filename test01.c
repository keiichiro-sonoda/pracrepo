#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(SEED);
    //srand((unsigned)time(NULL));
    initBoard();
    char format[] = FNF_L1RR05006000000005;
    //makeFGFilePrm1L(format);
    checkPrm1LFile(format, 1);
    // 500 世代に挑戦
    //nGenePrm1LLoopSeed(rltUniRd, format, 0, 0, 500);
    // for debugging
    float sample1[] = {-0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3};
    int l1 = arrayLength(sample1);
    float sample2[l1];
    int sample3[] = {51, -1, 2, 4, -100, 20, 50, -4, 52};
    int l2 = arrayLength(sample3);
    for (int i = 0; i < l1; i++)
        sample2[i] = ACT_FUNC(sample1[i]);
    printDecimal(l2);
    printFloatArray(sample2, l1);
    printString("yeah");
    return 0;
}