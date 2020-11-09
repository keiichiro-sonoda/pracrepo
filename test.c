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

    //checkSprmFile(FNF_05006000000005, 1);
    //makeFirstGeneFileFlex(FNF_05006000000005);
    //nGeneSSALoopFlex(nGeneSprmSaveAll, FNF_05006000000005, 0, 0, 100);
    sortTest();
    return 0;
}