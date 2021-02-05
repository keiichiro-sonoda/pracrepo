// this is a source file for debugging
// デバッグ用ファイル
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

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
    initSprm();
    srand(time(NULL));
    selDemo(exp(-0.1), 50, 3);
    puts("デバッグ中");
    return 0;
}