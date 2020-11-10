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

    //makeFirstGeneFileFlex(FNF_TEST);
    //checkSprmFile(FNF_TEST, 0);
    //nGeneSSALoopFlex(nGeneSprmSaveAll, FNF_05006000000005, 0, 0, 100);
    //sortTest();
    nGeneSSAFlexLoop(randAveUni, FNF_TEST, 1, 0, 2);
    return 0;
}