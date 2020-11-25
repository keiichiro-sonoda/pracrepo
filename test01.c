#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "genetic01.h"
#include "sort01.h"

int main(void) {
    srand(SEED);
    srand((unsigned)time(NULL));
    initBoard();
    char format[] = FNF_L1RR05006000000005;
    //makeFGFilePrm1L(format);
    //checkPrm1LFile(format, 1);
    // 500 世代に挑戦
    //nGenePrm1LLoopSeed(rltUniRd, format, 0, 0, 500);
    // for debugging
    printString(format);
    char c1, c2;
    float f1;
    for (int i = 0; i < 30; i++) {
        // 無理やりランダムなcharを作成
        c1 = randWeightChar();
        f1 = char2weight(c1);
        c2 = weight2char(f1);
        printf("%d, %f, %d\n", (int)c1, f1, (int)c2);
        if (c1 != c2)
            printf("NG!\n");
    }
    return 0;
}