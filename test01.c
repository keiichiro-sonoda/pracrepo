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
    int div = 255;
    int rslt[div];
    zeros(rslt, div);
    for (int i = 0; i < div * 10000; i++) {
        rslt[rand() % div]++;
    }
    printDecimalArray(rslt, div);
    return 0;
}