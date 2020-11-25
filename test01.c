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
    char format[] = FNF_TEST;
    //makeFGFilePrm1L(format);
    //checkPrm1LFile(format, 1);
    // 500 世代に挑戦
    //nGenePrm1LLoopSeed(rltUniRd, format, 0, 0, 500);
    // for debugging
    Prm1L pra[POPULATION];
    for (int i = 0; i < POPULATION; i++)
        randPrm1L(pra + i);
    showPrm1L(pra[4]);
    showPrm1L(pra[32]);
    dumpPrm1LComp(format, 0, pra);
    loadPrm1LComp(format, 0, pra);
    showPrm1L(pra[4]);
    showPrm1L(pra[32]);
    return 0;
}