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

// copy the first generation
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
    // check
    printFloatArray(pa[0].weight, SPRM_LEN);
    // file name for writing
    char fnamew[] = "prm/sprm_not_mutate000.bin";
    // open a file to write (or make a file)
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamew);
        return;
    }
    // opened!
    fwrite(pa, sizeof pa, 1, fp);
    // close
    fclose(fp);
}

// make next generation file
// for no mutant file
int nextGenerationSprmNM(int gene_num) {
    int i, j, count;
    // file name for not mutate
    char format[] = "prm/sprm_not_mutate%03d.bin";
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    // previous survivors
    Sprm parents[SURVIVE_NUM];
    FILE *fp;
    // print file name for reading
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    // print file name for writing
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
    // view
    printf("read file : %s\n", fnamer);
    printf("write file: %s\n", fnamew);
    // read parents
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        printf("\a%s can't be opened\n", fnamer);
        return -1;
    }
    // opened!
    fread(parents, sizeof parents, 1, fp);
    fclose(fp);
    // check write file (can read?)
    if ((fp = fopen(fnamew, "rb")) != NULL) {
        printf("\a%s exists. Can I overwrite it? (y): ", fnamew);
        fclose(fp);
        // don't overwrite
        if (getchar() != 'y') {
            printf("terminated\n");
            return -1;
        }
    }
    // make children!
    Sprm generation[GENE_NUM];
    // 10 parents copy
    for (count = 0; count < SURVIVE_NUM; count++) {
        generation[count] = parents[count];
    }
    // make children
    // 45 combinations, 2 children per couple
    for (i = 0; i < SURVIVE_NUM - 1; i++) {
        for (j = i + 1; j < SURVIVE_NUM; j++) {
            // average
            generation[count] = makeChildAverageSprm(parents[i], parents[j]);
            count++;
            // cross (not mutate)
            generation[count] = makeChildCrossNMSprm(parents[i], parents[j]);
            count++;
        }
    }
    // calcurate survivors
    Sprm survivors[SURVIVE_NUM];
    // battle!
    getSurvivorSprm(generation, survivors);
    // print parameters
    printString("top ten parameters:");
    for (i = 0; i < SURVIVE_NUM; i++) {
        printf("%2d: ", i + 1);
        printFloatArray(survivors[i].weight, SPRM_LEN);
    }

    // write current survivors
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        printf("\a%s cannot be opened\n", fnamew);
        return -1;
    }
    // opened!
    fwrite(survivors, sizeof survivors, 1, fp);
    fclose(fp);

    return 0;
}

int main(void) {
    // initial configurations
    initBoard();
    setIndexes();
    srand((unsigned)time(NULL));

    nextGenerationSprmNM(2);
    return 0;
}