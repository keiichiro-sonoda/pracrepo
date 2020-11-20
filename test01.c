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
    makeFGFilePrm1L(FNF_TEST);
    //checkPrm1LFile(FNF_L1R05006000000001, 2);
    //nGenePrm1LLoop(rltUniRd, FNF_L1R05006000000001, 1, 0, 100);
    float sample1[] = {-0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3};
    int l1 = arrayLength(sample1);
    float sample2[l1];
    for (int i = 0; i < l1; i++)
        sample2[i] = ACT_FUNC(sample1[i]);
    //printFloatArray(sample1, l1);
    //printFloatArray(sample2, l1);
    printString("yeah");
    return 0;
}