// this is a source file for debugging
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort01.h"
#include "genetic02.h"

int main(void) {
    srand(127U);
    srand((unsigned)time(NULL));
    setIndexes();
    makeSprmSample();
    showSprmOneLine(SAMP_PRM);
    Sprm children[2];
    Sprm samp2;
    float mut_rate = 0.05f;
    for (int i = 0; i < SPRM_LEN; i++)
        samp2.weight[i] = 0.111f;
    makeFirstGeneFileFlex(FNF_TEST);
    return 0;
}