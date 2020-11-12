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
    randPrm1L(&pt);
    return 0;
}