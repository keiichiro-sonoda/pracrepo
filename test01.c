#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(SEED);
    //srand((unsigned)time(NULL));
    initBoard();
    const char format[] = FNF_TEST;
    makeFGFilePrm1LComp(format);
    //makeFGFilePrm1L(format);
    //checkPrm1LFile(format, 1);
    // 500 世代に挑戦
    //nGenePrm1LLoopSeed(rltUniRd, format, 0, 0, 500);
    // for debugging
    char fname[FILENAME_MAX];
    snprintf(fname, FILENAME_MAX, format, 0);
    printString(fname);
    int fitness[POPULATION];
    srand(SEED);
    sortPrm1LCompFileByFitness(fname, fitness);
    Prm1L pra[POPULATION];
    loadPrm1LCompDirect(fname, pra);
    checkWinRatePrm1LVSRand(pra[0], 500);
    checkWinRatePrm1LVSRand(pra[49], 500);
    return 0;
}