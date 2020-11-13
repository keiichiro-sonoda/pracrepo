#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(123U);
    srand((unsigned)time(NULL));
    initBoard();
    makeFGFilePrm1L(FNF_TEST);
    Prm1L pra[POPULATION];
    loadPrm1L(FNF_TEST, 0, pra, sizeof pra);
    //showPrm1L(pra[2]);
    //leagueMatchPrm1LFlex(getBoardForBlackPrm1LRlt, pra, fitness);
    showPrm1L(pra[0]);
    showPrm1L(pra[1]);
    showPrm1L(uniCrossPrm1L(pra[0], pra[1]));
    return 0;
}