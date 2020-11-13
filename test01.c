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
    //makeFGFilePrm1L(FNF_TEST);
    //nGenePrm1L(rltUniRd, FNF_TEST, 0, 1);
    nGenePrm1LLoop(rltUniRd, FNF_TEST, 1, 0, 2);
    return 0;
}