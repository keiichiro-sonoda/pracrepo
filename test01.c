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
    checkPrm1LFile(FNF_NON_AF, 100);
    //nGenePrm1LLoop(rltUniRd, FNF_L1R05006000000001, 0, 0, 100);
    Prm1L pra[POPULATION];
    loadPrm1L(FNF_NON_AF, 100, pra, sizeof pra);
    oneToOneNPrm1LFlex(getBoardForBlackPrm1LRlt, pra[0], pra[1]);
    return 0;
}