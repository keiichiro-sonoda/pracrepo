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
    int loc_pop = 50, p_num = 3, count = 0, loop = 1000000;
    int parents[p_num], rslt[loc_pop];
    double cr = exp(8.0), geo_prog[loc_pop];
    geoProg(geo_prog, loc_pop, 1., cr);
    printFloatArrayExp(geo_prog, loc_pop);
    zeros(rslt, loc_pop);
    for (i = 0; i < loop; i++) {
        rouletteDoubleMltDep(geo_prog, loc_pop, parents, p_num);
        for (j = 0; j < p_num; j++) {
            rslt[parents[j]]++;
        }
    }
    printDecimalArray(rslt, loc_pop);
    puts("デバッグ中");
    return 0;
}