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

    //makeFirstGeneFileFlex(FNF_05006011000005);
    checkSprmFile(FNF_05006011000005, 0);
    nGeneSSAFlexLoop(randAveUni, FNF_05006011000005, 1, 0, 2);
    return 0;
}