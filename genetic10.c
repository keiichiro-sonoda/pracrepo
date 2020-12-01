#include <time.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    // 初期化にシード設定も含まれる
    initPrm1LComp();
    //srand((unsigned)time(NULL));
    //showBoard(START);
    const char format[] = FNF_TEST;
    printString(format);
    //makeFGFilePrm1LComp(format);
    //makeFGFilePrm1L(format);
    //checkPrm1LFile(format, 1);
    //nGenePrm1LLoopSeed(rltUniRd, format, 0, 0, 500);
    // for debugging
    //makeFGFilePrm1LComp(format);
    //nGenePrm1LCompLoop(rltUniRd, format, 1, 200, 401);
    Prm1L pr1, pr2, pr3;
    randPrm1L(&pr1);
    randPrm1L(&pr2);
    showPrm1L(pr1);
    showPrm1L(pr2);
    printf("crossover\n");
    pr3 = uniCrossBlockPrm1L(pr1, pr2);
    showPrm1L(pr3);
    randMutPrm1L(&pr3);
    printf("mutated?\n");
    showPrm1L(pr3);
    return 0;
}