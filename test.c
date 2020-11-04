// this is a source file for debugging
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

int main(void) {
    srand(123U);
    //srand((unsigned)time(NULL));
    setIndexes();
    makeSprmSample();
    showSprm(SAMP_PRM);
    Sprm children[2];
    Sprm samp2;
    int mut_rate = 0.05;
    for (int i = 0; i < SPRM_LEN; i++)
        samp2.weight[i] = 0.111;
    singlePointCrossover(SAMP_PRM, samp2, children, mut_rate);
    showSprmOneLine(SAMP_PRM);
    showSprmOneLine(samp2);
    showSprmOneLine(children[0]);
    showSprmOneLine(children[1]);
    return 0;
}