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
    int loc_pop = 50, count = 0, loop = 1000000;
    int parents[2], rslt[loc_pop];
    double cr = exp(-4.0), geo_prog[loc_pop];
    geoProg(geo_prog, loc_pop, 1., cr);
    printFloatArrayExp(geo_prog, loc_pop);
    parents[1] = 49;
    zeros(rslt, loc_pop);
    for (i = 0; i < loop; i++) {
        rouletteDoubleMltDep(geo_prog, loc_pop, parents, 2);
        for (j = 0; j < 2; j++) {
            rslt[parents[j]]++;
        }
    }
    printDecimalArray(rslt, loc_pop);
    puts("デバッグ中");
    return 0;
}