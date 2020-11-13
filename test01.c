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
    //showPrm1L(pra[2]);
    //leagueMatchPrm1LFlex(getBoardForBlackPrm1LRlt, pra, fitness);
    nGenePrm1L(rltUniRd, FNF_TEST, 0, 1);
    return 0;
}