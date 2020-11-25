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
    char c;
    float f = randWeight();
    f = 0.4999999f;
    printFloat(f);
    c = weight2char(f);
    printCharDecimal(c);
    f = char2weight(c);
    printFloat(f);
    c = weight2char(f);
    printCharDecimal(c);
    for (c = -127; c < 127; c++) {
        if (c != weight2char(char2weight(c))) {
            printf("no!!!\n");
        }
    }
    return 0;
}