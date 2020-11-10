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
    //makeFirstGeneFileFlex(FNF_10010011000005);
    //checkSprmFile(FNF_10010011000005, 0);
    nGeneSSAFlexLoop(randAveUni, FNF_10010011000005, 1, 100, 2);
    return 0;
}