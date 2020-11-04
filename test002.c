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
    int mut_rate = 0.01;
    for (int i = 0; i < SPRM_LEN; i++)
        samp2.weight[i] = i * 0.01;
    singlePointCrossover(SAMP_PRM, samp2, children, mut_rate);
    printFloatArray(children[0].weight, SPRM_LEN);
    printFloatArray(children[1].weight, SPRM_LEN);
    return 0;
}