#include <time.h>
#include <math.h>
#include <string.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(SEED);
    //srand((unsigned)time(NULL));
    initPrm1LComp();
    //showBoard(START);
    const char format[] = FNF_TEST;
    printString(format);
    //makeFGFilePrm1LComp(format);
    //makeFGFilePrm1L(format);
    //checkPrm1LFile(format, 1);
    // 500 世代に挑戦
    //nGenePrm1LLoopSeed(rltUniRd, format, 0, 0, 500);
    // for debugging
    //makeFGFilePrm1LComp(format);
    //nGenePrm1LComp(rltUniRd, format, 0, SEED, SEED * 2, 1);
    u_int s1;
    s1 = rand();
    printDecimal(s1);
    srand(s1);
    s1 = rand();
    printDecimal(s1);
    return 0;
}