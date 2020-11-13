#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(124U);
    srand((unsigned)time(NULL));
    initBoard();
    //makeFGFilePrm1L(FNF_TEST);
    Prm1L pra[POPULATION];
    int fitness[POPULATION];
    loadPrm1L(FNF_TEST, 0, pra, sizeof pra);
    //showPrm1L(pra[2]);
    //leagueMatchPrm1LFlex(getBoardForBlackPrm1LRlt, pra, fitness);
    float test1[] = {0.1, 0.2, 0.3, 0.4, 0.5};
    int l1 = arrayLength(test1);
    float test2[l1];
    zerosFloat(test2, l1);
    float test3[l1];
    uniCrossArray(test1, test2, test3, l1);
    printFloatArray(test1, l1);
    printFloatArray(test2, l1);
    printFloatArray(test3, l1);
    return 0;
}