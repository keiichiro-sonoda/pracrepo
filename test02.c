// this is a source file for debugging
// デバッグ用ファイル
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

typedef char* char_p;

// 選択のデモ用関数
void selDemo(double cr, int loc_pop, int n) {
    puts("選択のデモ");
    int parents[2];
    double geo_prog[loc_pop];
    geoProg(geo_prog, loc_pop, 1., cr);
    for (int i = 0; i < n; i++) {
        rouletteDoubleMltDep(geo_prog, loc_pop, parents, 2);
        printDecimalArray(parents, 2);
    }
}

int main(void) {
    setCORR_TABLE();
    initBoard();
    srand(time(NULL));
    char format[FILENAME_MAX];
    makeSprmFileFormatAuto(format, FILENAME_MAX, 0b010, 1, 50, 0, 3, 5, 0, .01, 123, -0.1, 0);
    changeDirPath(format, FILENAME_MAX, DIR02);
    puts(format);
    puts("デバッグ中");
    return 0;
}