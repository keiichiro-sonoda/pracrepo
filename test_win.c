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
    //sortTest();
    checkSprmFileFlex(FNF_05006000000005, 103, 50);
    return 0;
}