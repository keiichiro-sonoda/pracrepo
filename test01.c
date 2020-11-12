#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(123U);
    //srand((unsigned)time(NULL));
    initBoard();
    int ta[MASU_NUM + 1];
    board2arrayPlus(START, ta);
    printDecimalArray(ta, MASU_NUM + 1);
    return 0;
}