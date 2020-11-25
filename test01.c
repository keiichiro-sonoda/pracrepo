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
    sortPrm1LCompFileByFitness(fname, fitness);
    return 0;
}