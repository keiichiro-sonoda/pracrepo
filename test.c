// this is a source file for debugging
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

int main(void) {
    srand(123U);
    //srand((unsigned)time(NULL));
    setIndexes();
    initBoard();

    //makeFirstGeneFileFlex(FNF_05006000000005);
    checkSprmFile(FNF_05006000000005, 0);
    //checkSprmFile(FNF_TEST, 0);
    //nGeneSSALoopFlex(nGeneSprmSaveAll, FNF_05006000000005, 1, 0, 3);

    Sprm ptest[POPULATION];
    if (getSprmFile(FNF_05006000000005, 0, ptest, sizeof ptest) < 0)
        return -1;
    checkSprmStatistics(ptest, POPULATION);
    printf("ha?\n");
    return 0;
}