#include <stdio.h>
#include <stdlib.h>
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
    char format[] = "prm/simple_prm%03d.bin";
    char fnamer[FILENAME_MAX];
    int gene_num = 0;
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        printf("%s can't be opened.\n", fnamer);
        return;
    }
    // opened!
    Sprm pa[SURVIVE_NUM];
    fread(&pa, sizeof pa, 1, fp);
    fclose(fp);
    // check the top parameter
    showSprm(pa[0]);
    showFloatArray(pa[0].weight, SPRM_LEN);
    char fnamew[] = "prm/simple_prm000.bin";
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
    initBoard();
    sortTest();
    showBoard(START);
    return 0;
}