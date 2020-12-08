#include <time.h>
#include <math.h>
#include <string.h>
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
    Board bt;
    bt = START;
    showBoard(bt);
    emptyBoard(&bt);
    showBoard(bt);
    //play();
    return 0;
}