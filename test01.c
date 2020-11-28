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
    const char format[] = FNF_L1RRCM3205006000000001;
    printString(format);
    //makeFGFilePrm1LComp(format);
    //makeFGFilePrm1L(format);
    //checkPrm1LFile(format, 1);
    // 500 世代に挑戦
    //nGenePrm1LLoopSeed(rltUniRd, format, 0, 0, 500);
    // for debugging
    srand(SEED);
    //nGenePrm1LComp(rltUniRd, format, 0, 1);
    char str[81];
    makeFitnessFileName(str, sizeof str, format, 0);
    return 0;
}