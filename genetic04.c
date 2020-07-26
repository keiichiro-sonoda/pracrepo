// fix the corner evaluation
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "genetic02.h"

// define the corner evaluation
#define CORNER_EVAL 0.5

// copy the first generation
// and set the corner evaluation 0.5
void copyFGFixCorner1(void) {
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
    // check parameters
    for (int i = 0; i < SURVIVE_NUM; i++) {
        printFloatArray(pa[i].weight, SPRM_LEN);
        // fix the corner
        pa[i].weight[0] = CORNER_EVAL;
    }
    printString("fix!!");
    // reconfirmation
    for (int i = 0; i < SURVIVE_NUM; i++) {
        printFloatArray(pa[i].weight, SPRM_LEN);
    }
    // file name for writing
    char fnamew[] = "prm/sprm_corner0.5_000.bin";
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

// calculate average (fix the corner)
Sprm makeChildAverageSprmFC1(Sprm mother, Sprm father) {
    Sprm child;
    // the corner
    child.weight[0] = CORNER_EVAL;
    // the others
    for (int i = 1; i < SPRM_LEN; i++) {
        child.weight[i] = (mother.weight[i] + father.weight[i]) / 2;
    }
    return child;
}

// cross parameters (fix the corner)
// the mutation rate is 5%
Sprm makeChildCrossMSprmFC1(Sprm mother, Sprm father) {
    Sprm child;
    // the corner
    child.weight[0] = CORNER_EVAL;
    // the others
    for (int i = 1; i < SPRM_LEN; i++) {
        // rate 5%
        child.weight[i] = fcrossMFlex(mother.weight[i], father.weight[i], 0.05);
    }
    return child;
}

// make next generation file
// fix the evaluation value of corner to 0.5
int nextGenerationSprmFC1(int gene_num) {
    int i, j, count;
    // file name for fix the corner
    char format[] = "prm/sprm_corner0.5_%03d.bin";
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
            printString("terminated.");
            return -1;
        }
    }
    // make children!
    Sprm generation[GENE_NUM];
    // 10 parents copy
    for (count = 0; count < SURVIVE_NUM; count++) {
        generation[count] = parents[count];
    }
    // make children (fix the corner)
    // 45 combinations, 2 children per couple
    for (i = 0; i < SURVIVE_NUM - 1; i++) {
        for (j = i + 1; j < SURVIVE_NUM; j++) {
            // average
            generation[count] = makeChildAverageSprmFC1(parents[i], parents[j]);
            count++;
            // cross (not mutate)
            generation[count] = makeChildCrossMSprmFC1(parents[i], parents[j]);
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

// loop several times
void nextGenerationSprmFC1L(int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    for (int i = st; i < st + loop; i++) {
        nextGenerationSprmFC1(i);
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
    }
}
int main(void) {
    // initial configurations
    setIndexes();
    initBoard();
    srand((unsigned)time(NULL));

    nextGenerationSprmFC1L(2, 8);
    return 0;
}