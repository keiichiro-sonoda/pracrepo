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
    Prm1L pt;
    float at[PRM1L_LEN];
    randPrm1L(&pt);
    showPrm1L(pt);
    printFloat(evalWithPrm1L(START, pt));
    return 0;
}