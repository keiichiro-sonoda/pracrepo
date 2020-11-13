#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(127U);
    //srand((unsigned)time(NULL));
    initBoard();
    //makeFGFilePrm1L(FNF_TEST);
    //showPrm1L(pra[2]);
    //leagueMatchPrm1LFlex(getBoardForBlackPrm1LRlt, pra, fitness);
    //nGenePrm1L(rltUniRd, FNF_TEST, 0, 1);
    float test1[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    int l1 = arrayLength(test1);
    float test2[l1], test3[l1];
    zerosFloat(test2, l1);
    printFloatArray(test1, l1);
    printFloatArray(test2, l1);
    uniCrossArray(test1, test2, test3, l1);
    printFloatArray(test3, l1);
    randMutArray(test3, l1);
    printFloatArray(test3, l1);
    return 0;
}