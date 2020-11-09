// デバッグ用ファイル?
#include <stdio.h>
#include <stdlib.h>
#include "genetic02_win.h"
#include "sort01_win.h"

int main(int argc, char **argv) {
    // seed reset
    srand(123U);
    //srand((unsigned)time(NULL));
    // set global variable
    setCORR_TABLE();
    // set initial board
    initBoard();
    showBoard(START);
    checkSprmFile(0);
    //sortTest();
    Sprm ptest[50];
    if (loadSprmFileFlex(FNF_05006000000005, 0, ptest, sizeof ptest) >= 0)
        showSprm(ptest[0]);
    return 0;
}