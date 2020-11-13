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
    Prm1L pr1, pr2;
    randPrm1L(&pr1);
    randPrm1L(&pr2);
    oneToOneNPrm1LFlex(getBoardForBlackPrm1LRlt, pr1, pr2);
    return 0;
}