#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "genetic02.h"
#include "sort01.h"

// cross parameters (don't mutate)
Sprm makeChildCrossNMSprm(Sprm mother, Sprm father) {
    Sprm child;
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        // rate 0%
        child.weight[i] = fcrossMFlex(mother.weight[i], father.weight[i], 0.0);
    }
    return child;
}

void copyFirstGeneration(void) {
    FILE *fp;
    // file name for reading
    char fnamer[] = "prm/simple_prm000.bin";
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamer);
        return;
    }
    // opened!
    Sprm pa[SURVIVE_NUM];
    fread(pa, sizeof pa, 1, fp);
    fclose(fp);
    // check the top parameter
    showSprm(pa[0]);
    showFloatArray(pa[0].weight, SPRM_LEN);
    // file name for writing
    char fnamew[] = "prm/sprm_not_mutate000.bin";
    int i;
    Sprm pra[10];
    // open a file to write (or make a file)
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamew);
        return;
    }
    // opened!
    // random parameters
    for (i = 0; i < 10; i++)
        randSprm(pra + i);
    // check
    showSprm(pra[3]);
    // check size 800B?
    printSize(pra);
    fwrite(pra, sizeof pra, 1, fp);
    // close
    fclose(fp);
}

int main(void) {
    // initial configurations
    initBoard();
    setIndexes();
    srand((unsigned)time(NULL));
    // test
    showBoard(START);
    printDecimalArray(INDEXES, MASU_NUM);
    printDecimal(rand());
    return 0;
}