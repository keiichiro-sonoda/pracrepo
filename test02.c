// this is a source file for debugging
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

int main(void) {
    srand(SEED);
    srand((unsigned)time(NULL));
    setIndexes();
    initBoard();
    int pair[2];
    int points[6] = {6, 5, 4, 3, 2, 1};
    //randIntDoubleDep(pair, 1, 6);
    rouletteIntMltDep(points, 6, pair, 2);
    printDecimalArray(pair, 2);
    printString("debugging");
    return 0;
}