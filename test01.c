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
    Prm1L pra[POPULATION];
    int fitness[POPULATION];
    loadPrm1L(FNF_TEST, 0, pra, sizeof pra);
    int test[2];
    //showPrm1L(pra[2]);
    //leagueMatchPrm1LFlex(getBoardForBlackPrm1LRlt, pra, fitness);
    float a = 0.2f;
    float b = -0.33f;
    float tmp;
    zeros(test, 2);
    for (int i = 0; i < 10000; i++) {
        tmp = alternative(a, b);
        if (tmp == a) {
            test[0]++;
        } else if (tmp == b) {
            test[1]++;
        }
    }
    printDecimalArray(test, 2);
    return 0;
}