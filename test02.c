// this is a source file for debugging
// デバッグ用ファイル
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

int main(void) {
    initSprm();
    int loc_pop = 50;
    double cr = .1, geo_prog[loc_pop];
    geoProg(geo_prog, loc_pop, 1., cr);
    printFloatArrayExp(geo_prog, loc_pop);
    printString("debugging");
    return 0;
}