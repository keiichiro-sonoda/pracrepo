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
    const char format[] = FNF_L1RRCM3205006000000001;
    printString(format);
    //makeFGFilePrm1LComp(format);
    //makeFGFilePrm1L(format);
    //checkPrm1LFile(format, 1);
    //nGenePrm1LLoopSeed(rltUniRd, format, 0, 0, 500);
    // for debugging
    //makeFGFilePrm1LComp(format);
    //nGenePrm1LCompLoop(rltUniRd, format, 1, 200, 401);
    int sgl = 10;
    float sg1[sgl], sg2[sgl], sg3[sgl];
    randWeightArray(sg1, sgl);
    randWeightArray(sg2, sgl);
    uniCrossArray(sg1, sg2, sg3, sgl);
    printFloatArray(sg1, sgl);
    printFloatArray(sg2, sgl);
    printFloatArray(sg3, sgl);
    return 0;
}