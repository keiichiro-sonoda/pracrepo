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
    showBoard(START);
    //play();
    int ad, nad;
    for (int i = 0; i < MASU_NUM; i++) {
        ad = i << 1;
        for (int j = 0; j < 8; j++) {
            nad = DIRECTION[j];
            if (ifNeighbor(ad, nad) != isNeighbor(ad, nad)) {
                printf("%d, %d\n", ad, nad);
            }
        }
    }
    return 0;
}