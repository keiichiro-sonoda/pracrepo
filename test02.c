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
    int loc_pop = 50, count = 0, loop = 100000;
    int parents[2];
    double cr = .1, geo_prog[loc_pop];
    geoProg(geo_prog, loc_pop, 1., cr);
    printFloatArrayExp(geo_prog, loc_pop);
    for (int i = 0; i < loop; i++) {
        rouletteDoubleMltDep(geo_prog, loc_pop, parents, 2);
        randomizedQuicksortAll(parents, 2);
        if (parents[0] == 3 || parents[1] == 3) {
            count++;
        }
    }
    printf("%d / %d\n", count, loop);
    puts("デバッグ中");
    return 0;
}