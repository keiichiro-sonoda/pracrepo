// this is a source file for debugging
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

int main(void) {
    // シード設定 (再現可能性のため)
    srand(SEED);
    //srand((unsigned)time(NULL));
    // 初期化関数
    setIndexes();
    initBoard();
    checkSprmFile(FNF_05006000000005, 100);
    printString("debugging");
    return 0;
}