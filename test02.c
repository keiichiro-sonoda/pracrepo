// this is a source file for debugging
// デバッグ用ファイル
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

int main(void) {
    initSprm();
    srand(time(NULL));
    int i, j;
    int loc_pop = 50, p_num = 2, loop = 24;
    int parents[p_num], rslt[loc_pop];
    double cr = exp(-1), geo_prog[loc_pop];
    geoProg(geo_prog, loc_pop, 1., cr);
    printFloatArrayExp(geo_prog, loc_pop);
    for (i = 0; i < loop; i++) {
        rouletteDoubleMltDep(geo_prog, loc_pop, parents, p_num);
        printDecimalArray(parents, p_num);
    }
    puts("デバッグ中");
    return 0;
}