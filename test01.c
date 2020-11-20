#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(123U);
    //srand((unsigned)time(NULL));
    initBoard();
    char format[] = FNF_TEST;
    //makeFGFilePrm1L(format);
    //checkPrm1LFile(format, 0);
    //nGenePrm1LLoop(rltUniRd, format, 1, 0, 0);
    float sample1[] = {-0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3};
    int l1 = arrayLength(sample1);
    float sample2[l1];
    int sample3[] = {51, -1, 2, 4, -100, 20, 50, -4, 52};
    int l2 = arrayLength(sample3);
    for (int i = 0; i < l1; i++)
        sample2[i] = ACT_FUNC(sample1[i]);
    printFloatArray(sample1, l1);
    printFloatArray(sample2, l1);
    printDecimal(getMaxIntArray(sample3, l2));
    printString("yeah");
    return 0;
}