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
    char str[PRM1L_LEN];
    randWeightCharArray(str, PRM1L_LEN);
    for (int i = 0; i < PRM1L_LEN; i++)
        printf("%d ", (int)str[i]);
    putchar(10);
    return 0;
}